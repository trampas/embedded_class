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
#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#ifdef __cplusplus
#include "./drivers/pin/pin.h"
#include "libraries/syslog/syslog.h"

#endif 
#include "drivers/system/system.h"
#include "drivers/systick/systick.h"
#include <stdbool.h>
#include <stddef.h>
#include "sam.h"


//We are going to assume the M4 Metro 
//https://www.adafruit.com/product/3382

//LED Pin configuration
#define PIN_RXLED   		((const pin_t){PIN_PB06, NULL, PERPIHERAL_GPIO_OUTPUT_HIGH, 0, PERPIHERAL_GPIO_OUTPUT_HIGH})
#define PIN_TXLED       	((const pin_t){PIN_PA27, NULL, PERPIHERAL_GPIO_OUTPUT_HIGH, 0, PERPIHERAL_GPIO_OUTPUT_HIGH})

// UART
#define PIN_UART_TX			((const pin_t){PIN_PA22, SERCOM3, PERIPHERAL_MUX_D, PAD_0, PERPIHERAL_GPIO_INPUT_PULLED_LOW})
#define PIN_UART_RX			((const pin_t){PIN_PA23, SERCOM0, PERIPHERAL_MUX_D, PAD_1, PERPIHERAL_GPIO_INPUT_PULLED_LOW})





#define CPU_FREQUENCY                     (120000000ul)

//this is the frequency of the GCLKs, they are set in the startup before main is called.
static const uint32_t GCLK_freq[]= {TO_MHZ(120), TO_MHZ(12), TO_MHZ(100), 32768, TO_MHZ(48), TO_MHZ(9),0,TO_MHZ(1)};

// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_12M       (1u)
#define GENERIC_CLOCK_GENERATOR_12M_SYNC   GCLK_SYNCBUSY_GENCTRL1
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /* Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_1M		  (8u)
#define GENERIC_CLOCK_GENERATOR_1M_SYNC	  GCLK_SYNCBUSY_GENCTRL8
#define GENERIC_CLOCK_GENERATOR_48M		  (4u)
#define GENERIC_CLOCK_GENERATOR_48M_SYNC	GCLK_SYNCBUSY_GENCTRL4
#define GENERIC_CLOCK_GENERATOR_100M	  (5u)
#define GENERIC_CLOCK_GENERATOR_100M_SYNC	GCLK_SYNCBUSY_GENCTRL5
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (6u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K_SYNC   GCLK_SYNCBUSY_GENCTRL6
#define GENERIC_CLOCK_GENERATOR_9M       (7u)
#define GENERIC_CLOCK_GENERATOR_9M_SYNC   GCLK_SYNCBUSY_GENCTRL7

//USE DPLL0 for 120MHZ
#define MAIN_CLOCK_SOURCE				  GCLK_GENCTRL_SRC_DPLL0


#endif /* SRC_BOARD_H_ */
