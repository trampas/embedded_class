/**
 * \file
 *
 * \brief Low-level initialization functions called upon chip startup
 *
 * Copyright (c) 2017 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SYSTEM_SAMD21_H_INCLUDED_
#define _SYSTEM_SAMD21_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif
uint32_t getHeapUsed(void);
uint32_t getHeapSize(void);
size_t getMaxHeap(void);
int32_t getLastMalloc(void);
uint32_t getStackSize(void);
uint32_t getStackUsed(void);
uint32_t getStackStartOff(void);

uint32_t getLastFlashAddr(void);

#ifdef __cplusplus
} //extern "C"
#endif


extern uint32_t SystemCoreClock;   /*!< System Clock Frequency (Core Clock)  */

void SystemInit(void);
void SystemCoreClockUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_SAMD21_H_INCLUDED */
