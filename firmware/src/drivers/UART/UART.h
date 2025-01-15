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

#ifndef DRIVERS_UART_UART_H_
#define DRIVERS_UART_UART_H_


#include "board.h"
#include "drivers/pin/pin.h"
#include "libraries/fifo/fifo.h"
#include "drivers/NanoDMA/NanoDMA.h"
#include "drivers/systick/systick.h"
#include "devices/chardevice.h"

#define UART_FIFO_SIZE_TX (64)
#define UART_FIFO_SIZE_RX (64)

typedef enum {
	UART_PARITY_NONE=0,
	UART_PARITY_EVEN=1,
	UART_PARITY_ODD=2
} uart_parity_t;

typedef enum {
	UART_1_STOP_BITS=1,
	UART_2_STOP_BITS=2
} uart_stop_bits_t;

typedef void (*uart_callback)(uint8_t data);


class UART : public CharDevice {
public:
	UART();
	bool setup(const pin_t tx, const pin_t rx, uint32_t baud, uint8_t nvic_priority=LOWEST_IRQ_PRIORITY);
	
	size_t write(const uint8_t *ptrData, size_t numberBytes);
	size_t read(uint8_t *ptrData, size_t numberBytes);
	size_t available(void);
	void flush_tx(void); 
	
	void set_rx_callback(uart_callback callback);
	bool setBaud(uint32_t baud);
	uint32_t getBaud(void) { return _baud;}
	bool setStopBits(uart_stop_bits_t stop_bits);
	uart_stop_bits_t getStopBits();
	bool setParity(uart_parity_t parity_bits);
	uart_parity_t getParity();
	bool setDataBits(uint8_t bits);
	uint8_t getDataBits();
	
private:
	static void isrHandler(void *ptr);
	void resync();
	uart_callback _callback=NULL;
	Sercom *_ptrSercom;
	uint32_t _baud;
	uint32_t _framing_errors;
	uint8_t _irq_priority;
	bool _reset(void); 
	volatile bool _in_progress;
	void _dma_write() __attribute__(( optimize("-Ofast") ));
	void _wait_transfer_done();
	
	volatile uint32_t _tx_buf_num;
	volatile uint32_t _tx_buf_index;
	__attribute__ ((aligned (8))) uint8_t _dma_buff[UART_FIFO_SIZE_TX];
	uint8_t _tx_buff[2][UART_FIFO_SIZE_TX];
	NanoDMA _tx_dma;

	
	//FIFO<uint8_t, UART_FIFO_SIZE_TX> txFifo;
	FIFO<uint8_t, UART_FIFO_SIZE_RX> rxFifo;
friend void txDMADone(void *ptrClass, NanoDMAInterruptFlag_t flag);
};


#endif /* DRIVERS_UART_UART_H_ */
