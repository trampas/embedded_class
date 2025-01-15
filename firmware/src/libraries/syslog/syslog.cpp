/*******************************************************************
Copyright (C) 2025  MisfitTech LLC

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************/
#include "board.h"
#include "syslog.h"

#include "drivers/UART/UART.h"
#include "libraries/libc/vsnprintf.h"
#include "drivers/system/system.h"

#include "drivers/systick/systick.h"


#define ANSI_WHITE 		"\033[37m"
#define ANSI_NORMAL 	"\033[0m"
#define ANSI_BLINK 		"\033[5m"
#define ANSI_BLUE 		"\033[34m"
#define ANSI_MAGENTA 	"\033[35m"
#define ANSI_CYAN 		"\033[36m"
#define ANSI_WHITE 		"\033[37m"
#define ANSI_RED 		"\033[31m"
#define ANSI_GREEN 		"\033[32m"
#define ANSI_PINK 		"\033[35m\033[1m"
#define ANSI_BROWN 		"\033[33m"
#define ANSI_YELLOW 	"\033[33m\033[1m"
#define ANSI_BLACK      "\033[30m"
#define ANSI_BELL_AND_RED 		"\a\033[31m"

#define NEW_LINE "\n\r"

bool USBEnabled=false;

bool SysLogUSB(void) {return USBEnabled;}
void SysLogSetUSB(bool enabled) {USBEnabled=enabled;}

//UART dbgUART(DBG_TX,DBG_RX,115200);

CharDevice *ptrSerial[SYSLOG_MAX_UARTS]={0};
eLogLevel SyslogLevelToWrite;

static char buffer[SYSLOG_BUFFER_SIZE];
static unsigned int BufIndex=0;

static int SysLog_Enabled=1;

int SysLogDisable(void)
{
  SysLog_Enabled=0;
  return 0;
}

int SysLogEnable(void)
{
  SysLog_Enabled=1;
  return 0;
}

int SysLogIsEnabled(void)
{
  return SysLog_Enabled;
}


static void SysLogPuts(const char *ptrStr)
{

  uint32_t n,i;
  char *ptr;

  if (!SysLog_Enabled)
    return;


  for (i=0; i<SYSLOG_MAX_UARTS; i++)
  {
    if (ptrSerial[i])
    {
      n=0;
      ptr=(char *)ptrStr;
      while (n< strlen(ptr))
      {
        n=ptrSerial[i]->write((const uint8_t *)ptr,strlen(ptr));
        if (0 == n)
        {
        	break;
        }
        ptrSerial[i]->flush_tx();
        ptr=&ptr[n];
      }    
    }
  }
}

size_t syslog_write(const uint8_t *ptrdata, size_t n);

size_t syslog_write(const uint8_t *ptrdata, size_t n)
{
  uint32_t i;
  
//  if (HAS_DEBUGGER())
//  {
//	  SEGGER_RTT_Write(0,ptrdata, n);
//  }

   
  for (i=0; i<SYSLOG_MAX_UARTS; i++)
  {
    if (ptrSerial[i])
    {
      ptrSerial[i]->write(ptrdata,n);
      ptrSerial[i]->flush_tx();
    }
  }
  return n;
}

int SysLogInitDone=0;
void SysLogInit(CharDevice *ptrSerialObj, eLogLevel LevelToWrite)
{
  uint32_t i;
  for (i=0; i<SYSLOG_MAX_UARTS; i++)
  {
    if (NULL==ptrSerial[i])
    {
      ptrSerial[i]=ptrSerialObj;
      break;
    }
  }
//  
//  if (HAS_DEBUGGER())
//  {
//    	SEGGER_RTT_Init();
//  }

  if (SysLogInitDone == 0)
  {
    SyslogLevelToWrite=LevelToWrite;

    SysLogInitDone=1;
    BufIndex=0;
    memset(buffer,0,SYSLOG_BUFFER_SIZE);
  }
}


volatile int SysLogProcessing=0; // this is used such that syslog can be reentrent
volatile int SysLogMissed=0;


void SysLog(eLogLevel priorty, const char *fmt, ...)
{
  //UINT32 ret;
  int previousState=SysLog_Enabled;
  char vastr[MAX_SYSLOG_STRING]={0};
  //char outstr[MAX_SYSLOG_STRING]={0};


  va_list ap;

  if (SysLogProcessing)
  {
    //we have a syslog from a syslog call thus return as not much we can do...
    //memset(buffer,0,SYSLOG_BUFFER_SIZE);
    va_start(ap,fmt);
    _vsnprintf(&buffer[BufIndex],SYSLOG_BUFFER_SIZE-BufIndex,(const char *)fmt,ap);
    BufIndex=strlen(buffer);
    if (!(buffer[BufIndex-1] == '\n' || buffer[BufIndex-1] == '\r'))
    {
      _snprintf(&buffer[BufIndex],SYSLOG_BUFFER_SIZE-BufIndex,NEW_LINE);
      BufIndex=strlen(buffer);
    }
    SysLogMissed++; //set flag that we missed a call
    return;
  }

  SysLogProcessing=1;

  //stop the watch dog will doing a SysLog print
  Sys_WDogHoldOn();

  if(!SysLogInitDone)
  {
    //SysLogInit(NULL, LOG_WARNING); //not sure who is reseting serial port but before we print set it up
    WARNING("You should init SysLog");
    //SysLogInitDone=0;
  }

  //Send out a * that we missed a SysLog Message before this current message
  if (SysLogMissed)
  {
    //SysLogPuts(ANSI_RED);
    SysLogPuts("*** Reentrant Log call possible loss of message(s):");
    SysLogPuts(NEW_LINE);
    if (BufIndex>0)
    {
      SysLogPuts(buffer);
      memset(buffer,0,SYSLOG_BUFFER_SIZE);
      BufIndex=0;
    }
    //SysLogPuts(ANSI_RED);
    SysLogPuts("***********");
    SysLogPuts(NEW_LINE);
    SysLogMissed=0;
  }



  memset(vastr,0,MAX_SYSLOG_STRING);
  va_start(ap,fmt);
  //#ifndef PGM_P
#if 1
  _vsnprintf(vastr,MAX_SYSLOG_STRING,(char *)fmt,ap);
#else
  vsprintf_P(vastr,(const char *)fmt,ap);
#endif
  //get time and store in datetimestr if desired
  //sprintf(outstr, "[%s] %s\r\n", datetimestr, vastr);



  if (priorty<=LOG_ERROR)
  {
    SysLog_Enabled=1;
    SysLogPuts(ANSI_RED);

  }else if (priorty==LOG_DEBUG)
  {
    SysLogPuts(ANSI_WHITE);
  }else if (priorty==LOG_WARNING)
  {
    SysLogPuts(ANSI_BLUE);
  }

  //add factory seconds... 
     char datetimestr[MAX_SYSLOG_STRING]={0};
     _snprintf(datetimestr,sizeof(datetimestr),"%u:",(uint32_t)factorySeconds());
     SysLogPuts(datetimestr);
#ifdef RTC_H_
#ifdef TIME_H_
  {
    struct tm tp;
    RTC_Time_s tm;
    time_t secs;
    char datetimestr[MAX_SYSLOG_STRING]={0};

    RTC_ReadTime(&tm);
    secs=tm.seconds;
    convertFlexNetTime((time_t *)&secs, &tp);
    time_str(datetimestr,&tp);
    SysLogPuts(datetimestr);

    if (priorty<=SyslogLevelToWrite && SysLogWriteFunc!=NULL)
    {
      SysLogWriteFunc(datetimestr,strlen(datetimestr));
    }
  }
#endif
#endif

  while (vastr[strlen(vastr)-1] == '\n' || vastr[strlen(vastr)-1] == '\r')
  {
    vastr[strlen(vastr)-1]='\0';
  }

  SysLogPuts(vastr);


  //
  //    if (priorty<=SyslogLevelToWrite && SysLogWriteFunc!=NULL)
  //    {
  //    	SysLogWriteFunc(vastr,strlen(vastr));
  //    	SysLogWriteFunc(NEW_LINE,strlen(NEW_LINE));
  //    }
  SysLogPuts(ANSI_NORMAL);
  if (priorty != LOG_PRINT)
  {
    SysLogPuts(NEW_LINE);
  }




  if (priorty == LOG_EMERG) {
    //you can reboot processor here
  }

  //start the watch dog where left off..
  Sys_WDogHoldOff();
  SysLogProcessing=0;
  SysLog_Enabled=previousState;
  return;
}

