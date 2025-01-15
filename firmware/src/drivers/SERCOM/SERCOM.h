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
#ifndef DRIVERS_SERCOM_SERCOM_H_
#define DRIVERS_SERCOM_SERCOM_H_

#include "sam.h"
#include "board.h"

typedef bool bool_t;
typedef void (*SercomISR)(void *ptrParam);


bool disableInterrupt(Sercom *ptrSercom);
void restoreInterrupt(Sercom *ptrSercom, bool preState);
bool enableSercomNVICAndClocks(Sercom *ptrHw, SercomISR isrHandler, void *ptrCallbackParam, uint8_t nvic_priortiy=LOWEST_IRQ_PRIORITY);
uint8_t getSercomDMA_TX_ID(Sercom *ptrHw);

#endif /* DRIVERS_SERCOM_SERCOM_H_ */
