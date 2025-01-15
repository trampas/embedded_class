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
#ifndef DRIVERS_CHIP_DELAY_DELAY_H_
#define DRIVERS_CHIP_DELAY_DELAY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t SystemCoreClock;   /*!< System Clock Frequency (Core Clock)  */

static __inline__ void delay_us( uint32_t ) __attribute__((always_inline, unused)) ;
static __inline__ void delay_us( uint32_t usec )
{
  if ( usec == 0 )
  {
    return ;
  }

  /*
   *  The following loop:
   *
   *    for (; ul; ul--) {
   *      __asm__ volatile("");
   *    }
   *
   *  produce the following assembly code:
   *
   *    loop:
   *      subs r3, #1        // 1 Core cycle
   *      bne.n loop         // 1 Core cycle + 1 if branch is taken
   */

  // VARIANT_MCK / 1000000 == cycles needed to delay 1uS
  //                     3 == cycles used in a loop
  volatile uint32_t n = usec * (SystemCoreClock / 1000000UL) / 9;
  while (n--) {};
//  __asm__ __volatile__(
//    "1:              \n"
//    "   sub %0, #1   \n" // substract 1 from %0 (n)
//    "   bne 1b       \n" // if result is not 0 jump to 1
//    : "+r" (n)           // '%0' is n variable with RW constraints
//    :                    // no input
//    :                    // no clobber
//  );
  // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
  // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Volatile
}

static __inline__ void delay_ms( uint32_t ) __attribute__((always_inline, unused)) ;
static __inline__ void delay_ms( uint32_t ms )
{
	while(ms)
	{
		delay_us(1000);
		ms--;
	}
}

#ifdef __cplusplus
}
#endif


#endif /* DRIVERS_CHIP_DELAY_DELAY_H_ */
