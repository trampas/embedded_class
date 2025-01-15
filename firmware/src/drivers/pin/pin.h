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
#ifndef DRIVERS_PIN_PIN_H_
#define DRIVERS_PIN_PIN_H_

#include "sam.h"
#include <stdbool.h>
#include "drivers/system/system.h"

#pragma GCC diagnostic ignored "-Wconversion"
typedef enum {
	PIN_PERIPHERAL_MUX_A=0,
	PIN_PERIPHERAL_MUX_B=1,
	PIN_PERIPHERAL_MUX_C=2,
	PIN_PERIPHERAL_MUX_D=3,
	PIN_PERIPHERAL_MUX_E=4,
	PIN_PERIPHERAL_MUX_F=5,
	PIN_PERIPHERAL_MUX_G=6,
	PIN_PERIPHERAL_MUX_H=7,
	PIN_PERIPHERAL_MUX_I=8,
	PIN_PERIPHERAL_MUX_J=9,
	PIN_PERIPHERAL_MUX_K=0x0A,
	PIN_PERIPHERAL_MUX_L=0x0B,
	PIN_PERIPHERAL_MUX_M=0x0C,
	PIN_PERIPHERAL_MUX_N=0x0D,
} pinPeripheralMux_t;

typedef enum {
	PAD_0=0,
	PAD_1=1,
	PAD_2=2,
	PAD_3=3
} pinPad_t;


typedef enum {
	PERPIHERAL_NULL,
	PERPIHERAL_GPIO_INPUT,
	PERPIHERAL_GPIO_INPUT_PULLED_HIGH,
	PERPIHERAL_GPIO_INPUT_PULLED_LOW,
	PERPIHERAL_GPIO_OUTPUT_LOW,
	PERPIHERAL_GPIO_OUTPUT_HIGH,
	PERPIHERAL_EIC,
	PERPIHERAL_EIC_PULLED_HIGH,
	PERPIHERAL_EIC_PULLED_LOW,
	PERPIHERAL_REF,
	PERPIHERAL_ADC,
	PERPIHERAL_AC,
	PERPIHERAL_DAC,
	PERPIHERAL_PTC,
	PERPIHERAL_TC_TCC_OUTPUT,
	PERPIHERAL_TC_TCC_INPUT,
	PERPIHERAL_TCC_OUTPUT,
	PERPIHERAL_TCC_INPUT,
	PERPIHERAL_COM,
	PERPIHERAL_AC_GCL,
	PERPIHERAL_SDHC_CAN0,
	PERPIHERAL_CORTEX,
	PERIPHERAL_MUX_A,
	PERIPHERAL_MUX_B,
	PERIPHERAL_MUX_C,
	PERIPHERAL_MUX_D,
	PERIPHERAL_MUX_E,
	PERIPHERAL_MUX_F,
	PERIPHERAL_MUX_G,
	PERIPHERAL_MUX_G_OUTPUT,
	PERIPHERAL_MUX_H,
	PERIPHERAL_MUX_I,
	PERIPHERAL_MUX_J,
	PERIPHERAL_MUX_K,
	PERIPHERAL_MUX_L,
	PERIPHERAL_MUX_M,
	PERIPHERAL_MUX_N,
}pinMuxType_t;

typedef struct {
	uint32_t pinNum;   //this is a pin number, upper 16bits is port, lower is pin number
	void *ptrPerherial; //if using a peripheral, set this pointer to peripheral
	pinMuxType_t type;  //which pin mux is needed for peripheral
	uint32_t id; //pad, pin, waveform, adc channel, etc
	pinMuxType_t sleepType;  //what to do with pin when you sleep
}pin_t;

typedef enum {
	NONE=0,
	RISING_EDGE,
	FALLING_EDGE,
	BOTH_EDGES,
	HIGH_LEVEL,
	LOW_LEVEL,
}InterruptType_t;

#define PIN_NULL		((const pin_t){0, NULL, PERPIHERAL_NULL, 0, PERPIHERAL_NULL})

#define IS_PIN_NULL(x) ((x).pinNum==0 && (x).ptrPerherial==NULL && (x).type==PERPIHERAL_NULL && (x).sleepType==PERPIHERAL_NULL)

void PinEnableInterrupt(pin_t pin, InterruptType_t type, voidCallback_t callback);
bool PinDisableInterrupt(pin_t pin); //returns true if interrupt was enabled
void PinConfig(const pin_t pin);

void EICInit(void);

#define PIN_GET_PORT(x) ((x)>>5)
#define PIN_GET_PIN(x) ((x)&0x01F)


static inline bool PinRead(const pin_t pin) __attribute__((always_inline));
static inline bool PinRead (const pin_t pin)
{
	return (PORT->Group[PIN_GET_PORT(pin.pinNum)].IN.reg & (1<<PIN_GET_PIN(pin.pinNum)))!=0;

}

static inline void PinAnalog(const pin_t pin) __attribute__((always_inline));
static inline void PinAnalog (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].DIRCLR.reg=(1<<PIN_GET_PIN(pin.pinNum));
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg=0;
}

static inline void PinDefaults (const pin_t pin) __attribute__((always_inline));
static inline void PinDefaults (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg=0;
	PORT->Group[PIN_GET_PORT(pin.pinNum)].DIRCLR.reg=(1<<PIN_GET_PIN(pin.pinNum));
}

static inline void PinHigh (const pin_t pin) __attribute__((always_inline));
static inline void PinHigh (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].OUTSET.reg=(1<<PIN_GET_PIN(pin.pinNum));
}

static inline void PinLow (const pin_t pin) __attribute__((always_inline));
static inline void PinLow (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].OUTCLR.reg=(1<<PIN_GET_PIN(pin.pinNum));
}

static inline void PinEnableInputBuffer(const pin_t pin) __attribute__((always_inline));
static inline void PinEnableInputBuffer(const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].bit.INEN=1;
}

static inline void PinSetAsInput (const pin_t pin) __attribute__((always_inline));
static inline void PinSetAsInput (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].DIRCLR.reg=(1<<PIN_GET_PIN(pin.pinNum));
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].bit.INEN=1;
}

static inline void PinSetAsOutput (const pin_t pin) __attribute__((always_inline));
static inline void PinSetAsOutput (const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].DIRSET.reg=(1<<PIN_GET_PIN(pin.pinNum));
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg=0;
}


static inline void PinSetSetMux(const pin_t pin, const pinPeripheralMux_t mux) __attribute__((always_inline));
static inline void PinSetSetMux(const pin_t pin, const pinPeripheralMux_t mux)
{
	if (PIN_GET_PIN(pin.pinNum) & 0x001)
	{
		PORT->Group[PIN_GET_PORT(pin.pinNum)].PMUX[PIN_GET_PIN(pin.pinNum)/2].bit.PMUXO=mux;
	}else
	{
		PORT->Group[PIN_GET_PORT(pin.pinNum)].PMUX[PIN_GET_PIN(pin.pinNum)/2].bit.PMUXE=mux;
	}

	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg|=PORT_PINCFG_PMUXEN;

}

static inline void PinSetGPIOMux(const pin_t pin) __attribute__((always_inline));
static inline void PinSetGPIOMux(const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg &= ~(PORT_PINCFG_PMUXEN);
}

static inline void PinSetSetInputPulledHigh(const pin_t pin) __attribute__((always_inline));
static inline void PinSetSetInputPulledHigh(const pin_t pin)
{
	PinSetAsInput(pin);
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg=PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	PORT->Group[PIN_GET_PORT(pin.pinNum)].OUTSET.reg=(1<<PIN_GET_PIN(pin.pinNum));
}

static inline void PinSetSetInputPulledLow(const pin_t pin) __attribute__((always_inline));
static inline void PinSetSetInputPulledLow(const pin_t pin)
{
	PinSetAsInput(pin);
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].reg=PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
	PORT->Group[PIN_GET_PORT(pin.pinNum)].OUTCLR.reg=(1<<PIN_GET_PIN(pin.pinNum));
}

static inline void PinEnableStrongDrive(const pin_t pin) __attribute__((always_inline));
static inline void PinEnableStrongDrive(const pin_t pin)
{
	PORT->Group[PIN_GET_PORT(pin.pinNum)].PINCFG[PIN_GET_PIN(pin.pinNum)].bit.DRVSTR=1;
}

#pragma GCC diagnostic pop

#endif /* DRIVERS_PIN_PIN_H_ */
