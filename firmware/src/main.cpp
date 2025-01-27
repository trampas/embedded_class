/*
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
*/

#include "board.h"
#include "drivers/delay/delay.h"
#include "drivers/systick/systick.h"
#include "drivers/UART/UART.h"
#include "libraries/syslog/syslog.h"


int main()
{
  UART dbg_uart;
  //configure the systick timer 
  SysTickInit(NULL);

  dbg_uart.setup(PIN_UART_TX, PIN_UART_RX, 115200);

  SysLogInit(&dbg_uart, LOG_ALL);

  LOG("Power up");

  //configure the LED pins
  PinConfig(PIN_RXLED);
  PinConfig(PIN_TXLED);

  //turn LEDs on
  PinLow(PIN_RXLED);
  PinLow(PIN_TXLED);


  while(1)
  {
    //LOG("Hello World %d",millis());
    delay_ms(500);
    PinHigh(PIN_RXLED);
    delay_ms(500);
    PinLow(PIN_RXLED);
  }

 return 0;
}
