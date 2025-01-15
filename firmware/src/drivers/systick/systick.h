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
#ifndef DRIVERS_SYSTICK_SYSTICK_H_
#define DRIVERS_SYSTICK_SYSTICK_H_

#include "stdint.h"

typedef void (*voidCallback_t)(void);

//the callback will be called every 1ms based on systick timer.
void SysTickInit(voidCallback_t callback);


typedef struct {
	volatile uint32_t _millis;
	volatile uint32_t _delay;
	volatile voidCallback_t  _callback;
	volatile bool _triggered;
	volatile bool _oneShot;
	volatile void *ptrNext;
} systick_timer_t;



 bool addPeridoicTimer(systick_timer_t *ptrTimer, voidCallback_t callback, uint32_t delay_millis);
 bool addOneShotTimer(systick_timer_t *ptrTimer, voidCallback_t callback, uint32_t delay_millis);
 bool removeTimer(systick_timer_t *ptrTimer);
 bool rescheduleTimer(systick_timer_t *ptrTimer, uint32_t delay_millis);
 
 void setFactorySeconds(uint32_t sec);
 uint32_t factorySeconds(void); //this is always increasing seconds
 uint32_t porSeconds(void);
 uint32_t micros(void);
 uint32_t millis(void);


#endif /* DRIVERS_SYSTICK_SYSTICK_H_ */
