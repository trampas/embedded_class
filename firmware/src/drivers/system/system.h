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
#ifndef SYSTEM_H_
#define SYSTEM_H_

//#include <core_cmFunc.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "sam.h"
#include "stdbool.h"
#include "drivers/delay/delay.h"
#include "drivers/systick/systick.h"


//this is the lowest priority IRQ number
#define LOWEST_IRQ_PRIORITY ( (1<<__NVIC_PRIO_BITS) - 1)
#define HIGHEST_IRQ_PRIORITY (0)

#define UNUSED(x) (void)(x)

#define HIGH (true)
#define LOW (false)

#define BIT_SET(x,bit) ( (x) |= ((1)<<(bit)) )
#define BIT_CLR(x,bit) ( (x) &= (~((1)<<(bit))) )

#define ABS(x) (((x)<0)?(-(x)):(x))
#define SGN(x) (((x)<0)?(-1):(1)))

#define MAX(x,y) (((x)<(y))?(y):(x))
#define MIN(x,y) (((x)<(y))?(x):(y))

#define TO_MHZ(x)  ((x) * 1000000L) // use uppercase L to avoid confusion
#define TO_KHZ(x)  ((x) * 1000L) // use uppercase L to avoid confusion
#define TO_MSEC(x) ((x) / 1000L )   // with the digit 1

#define PACK            __attribute__((packed))
#define WEAK            __attribute__((weak))
#define INLINE          static inline __attribute__((always_inline))
#define ALIGN(x)		__attribute__ ((aligned (x)))
#define LIMIT(a, b)     (((int)(a) > (int)(b)) ? (int)(b) : (int)(a))

#define POINTER(T)  typeof(T *)
#define ARRAY(T, N) typeof(T [N])

#ifndef PI
//#define PI 3.14159265
#define PI 3.14159265f
#endif

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define MEMBER_SIZE(type, member) sizeof(((type *)0)->member)
#define OFFSETOF(type, member) (size_t)&(((type *)0)->member)

#define ASSERT_FAIL(msg) do { \
   assert(!msg);\
   } while (0)

#define ATOMIC_SECTION_ENTER   { bool __isrState =InterruptDisable();
#define ATOMIC_SECTION_LEAVE   InterruptEnable(__isrState); }

#define ZERO_ARRAY(x)  memset((void *)x,0,sizeof(x));

#define likely(x)  	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)



#define SIGN(x) (((x)>=0) ? (1) : (-1))

#pragma GCC diagnostic ignored "-Wconversion"
#define DIVIDE_WITH_ROUND(x, divisor)(          \
{                           \
    typeof(x) __x = x;              \
    typeof(divisor) __d = divisor;          \
    (((typeof(x))-1) > 0 ||             \
     ((typeof(divisor))-1) > 0 || (__x) > 0) ?  \
        (((__x) + ((__d) / 2)) / (__d)) :   \
        (((__x) - ((__d) / 2)) / (__d));    \
}                           \
)
#pragma GCC diagnostic pop

#define HAS_DEBUGGER() (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)

// NOTE: If you are using CMSIS, the registers can also be
// accessed through CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk
#define HALT_IF_DEBUGGING()                              \
  do {                                                   \
    if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) { \
      __asm("bkpt 1");                                   \
    }                                                    \
} while (0)


static inline char * STRNCPY(char *dst,const char *src,size_t n) {dst[n-1]='\0'; return strncpy(dst,src,n-1);}

//get the priority of the currently active interrupt
// if no interrupt is enable it will return -1
inline static uint32_t  GetISRPriority(void)
{
	int32_t x;
	x=__get_IPSR() & 0x0FF;
	x=x-16;
	if (x<-15 || x>=PERIPH_COUNT_IRQn)
	{
		return (uint32_t)(-1);
	}
	return NVIC_GetPriority((IRQn_Type)x);
}

//returns true if code is being ran in interrupt context. 
static inline bool ISRActive(void)
{
	uint32_t x;
	x=__get_IPSR() & 0x0FF;

	return x!=0;
}

static inline bool InterruptsEnabled(void)
{
	uint32_t prim;
	/* Read PRIMASK register, check interrupt status before you disable them */
    /* Returns 0 if they are enabled, or non-zero if disabled */
    prim = __get_PRIMASK();

    //note the above code looks like a race condition
    // however if interrupts state changed between the two above functions
    // it was from and interrupt and either way we will restore state before we
    // disabled interrupts.
    return (prim == 0);
}
/*
 *  Interrupt Disable routine which returns the state of interrupts
 *  before disabling
 *
 *  This disables all interrupts
 *
 * 	Returns true if interrupts were enabled
 */
static inline bool InterruptDisable(void)
{
	uint32_t prim;
	/* Read PRIMASK register, check interrupt status before you disable them */
    /* Returns 0 if they are enabled, or non-zero if disabled */
    prim = __get_PRIMASK();
      /* Disable interrupts */
    __disable_irq();

    //note the above code looks like a race condition
    // however if interrupts state changed between the two above functions
    // it was from and interrupt and either way we will restore state before we
    // disabled interrupts.
    return (prim == 0);
}


/*
 *  Interrupt enable routine which enables interrupt if passed a true
 *
 *  This enables global interrutps
 */
static inline void InterruptEnable(bool state)
{
	if (state) //if we state is true enable the interrupts
	{
		__enable_irq();
		__ASM volatile ("dsb" : : : "memory");
		__ASM volatile ("isb" : : : "memory");
	}
}

/*
 *  Theses are some simple functions to do a Mutex
 */

typedef volatile int Mutex_t;

static inline void MutexRelease(volatile Mutex_t *ptrMutex)
{
   bool  isrState=InterruptDisable();
   *ptrMutex=0;
   InterruptEnable(isrState);
}

static inline  bool MutexAcquire(volatile Mutex_t *ptrMutex)
{
   bool  isrState=InterruptDisable();
   if (*ptrMutex!=0)
   {
      InterruptEnable(isrState);
      return false; //did not get mute
   }
   *ptrMutex=1;
   InterruptEnable(isrState);
   return true; //got mutex
}

static inline char* lltoa(long long val, int base){

    static char buf[64] = {0};

    int i = 62;
    int sign = (val < 0);
    if(sign) val = -val;

    if(val == 0) return (char *)"0";

    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    if(sign) {
        buf[i--] = '-';
    }
    return &buf[i+1];

}


#ifdef __cplusplus
#include <functional>
//This function will time out if the function passed in does not return false
// before the microSeconds have passed
static inline bool waitTimeoutWhile(std::function<bool()> ptrFunc, uint32_t microSeconds)
#else
//This function will time out if the function passed in does not return false
// before the microSeconds have passed
static inline bool waitTimeoutWhile(bool (*ptrFunc)(), uint32_t microSeconds)
#endif
{

	//if interrupts are enabled we can use the systick timer
	if (InterruptsEnabled())
	{
		uint32_t to=micros();
		while(ptrFunc())
		{
			if ((micros()-to)>microSeconds)
			{
				return false;
			}
		}
		return true;
	}
	//if interrupts are not enabled we have to use spin loop delay
	// this will not be super accurate but...
	while(ptrFunc())
	{
		delay_us(1);
		if (microSeconds==0)
		{
			return false;
		}
		microSeconds--; //increment after in case someone passed in zero to function.
	}
	return true;
}


static inline bool _disable_cache(void)
{
  bool state=CMCC->CTRL.bit.CEN;
 // if (state){
	   CMCC->CTRL.bit.CEN = 0;
	   while ( CMCC->SR.bit.CSTS ) { }
	   CMCC->MAINT0.bit.INVALL = 1;
 // }
  return state;
}


static inline void _enable_cache(bool prev_state)
{
	if (prev_state){
		CMCC->CTRL.bit.CEN = 1;
	}
}

static inline void cache_invalidate(void)
{
	 bool state=_disable_cache();
	 _enable_cache(state);

}


#endif /* SYSTEM_H_ */
