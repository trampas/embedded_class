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
#include "UART.h"

#include <string.h>

#include "board.h"
#include "drivers/SERCOM/SERCOM.h"
#include "drivers/pin/pin.h"
#pragma GCC diagnostic ignored "-Wconversion"

void UART::isrHandler(void *ptr)
{

   uint8_t x=0;
   bool x_set=false;

   UART *ptrUart = (UART *)ptr;


   if (ptrUart->_ptrSercom->USART.INTFLAG.bit.RXC)
   {   
	   x = (uint8_t)ptrUart->_ptrSercom->USART.DATA.reg;
      x_set=true;
   }
   
   //if we have framing errors we will assume
   // we are sync'd wrong and reset the sync. 
   if (ptrUart->_ptrSercom->USART.INTFLAG.bit.ERROR)
   {
	   ptrUart->_framing_errors++;
	   if (ptrUart->_framing_errors>=2){
		   ptrUart->resync();
		   ptrUart->_framing_errors=0;
	   }
	  
   }else
   {
	   ptrUart->_framing_errors=0;
      if (x_set){
         ptrUart->rxFifo.push(&x);
      }
	   
   }
   
   if (ptrUart->_ptrSercom->USART.STATUS.bit.BUFOVF)
   {
      //ERROR("uart overflow");
      ptrUart->_ptrSercom->USART.STATUS.bit.BUFOVF = 1;
   }
 
   if (ptrUart->_callback != NULL){
	   (ptrUart->_callback)(x);
   }
}


void UART::set_rx_callback(uart_callback callback){
	_callback=callback; 
	
}

void UART::resync()
{
	static uint32_t count=0;
	
	count++;
	if (count>1000)
	{
		count=0;
	}
	
	
	//disable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 0;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return;
   }
   delay_us(count);
   
   //enable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 1;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return;
   }
     
   _framing_errors=0;
}

UART::UART()
{
   _ptrSercom = NULL;
   _tx_buf_num = 0;
   _tx_buf_index = 0;
   _in_progress = false;
   _framing_errors=0;
}

void UART::_wait_transfer_done(){
	
	//no data and no DMA in progress
	if (_tx_buf_index == 0 && !_tx_dma.isBusy())
	{
	  return;
	}
	
	//wait for DMA to complete
	uint32_t to = millis();
	while (_tx_dma.isBusy())
	{
	  if ((millis() - to) > 100)
	  {
		 //we have waited 100ms for UART to transmit data, something is wrong
		 _reset();
		 ERROR("UART ERROR");
		 return;
	  }
	}
	
} 


void  UART::_dma_write(){
	volatile uint32_t n, buf;
	
	_wait_transfer_done();
	
	if (_tx_buf_index == 0)
	{
		return; //no data to transmit. 
	}

	//we have to disable global interrupts 
	// as we allow UART to be called from interrupts. 
	bool isr_state = InterruptDisable();
	_in_progress = true;
	n = _tx_buf_index;
	buf = _tx_buf_num;
	_tx_buf_num++;
	if (_tx_buf_num >= 2)
	{
	  _tx_buf_num = 0;
	}
	_tx_buf_index = 0;
	_in_progress = false;

	memcpy(_dma_buff, &_tx_buff[buf][0], UART_FIFO_SIZE_TX);
	
	_tx_dma.config(4, getSercomDMA_TX_ID(_ptrSercom), DMAC_CHCTRLA_TRIGACT_BURST_Val, 0, false);
	
	_tx_dma.ptrUserData = (void *)this;
	// _tx_dma.setCallback(txDMADone);
	
	_tx_dma.setFirstTransfer(_dma_buff, (void *)&_ptrSercom->USART.DATA.reg,
							n, DMA_BEAT_SIZE_BYTE,
							true, false, NULL);
	
	__DSB();
	__ISB();
	_tx_dma.startTransfer(_irq_priority);  //start with correct interrupt priority.
	
	InterruptEnable(isr_state);
}

void UART::flush_tx(void)
{

  _wait_transfer_done();
  
  
  //if we have more data to write start writing.
  if (_tx_buf_index != 0)
  {
	  _dma_write();
	  _wait_transfer_done();
  }
   
}




bool UART::setStopBits(uart_stop_bits_t stop_bits)
{
	if (_ptrSercom == NULL)
   {
      return false;
   }
	if (stop_bits != UART_1_STOP_BITS && stop_bits != UART_2_STOP_BITS)
	{
		ERROR("Incorrect stop bits, only 1 or 2 supported");
		return false;
	}
	
   bool isrState = disableInterrupt(_ptrSercom);

   //save the enabled state.
   uint32_t bit = _ptrSercom->USART.CTRLA.bit.ENABLE;

   //disable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 0;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //set stop bits
   _ptrSercom->USART.CTRLB.bit.SBMODE = stop_bits;

   //restore the enable bit
   _ptrSercom->USART.CTRLA.bit.ENABLE = bit;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //restore the interrupt state
   restoreInterrupt(_ptrSercom, isrState);
   return true;
}

uart_stop_bits_t UART::getStopBits() 
{
	return (uart_stop_bits_t)_ptrSercom->USART.CTRLB.bit.SBMODE;
}
	
bool UART::setParity(uart_parity_t parity_bits)
{
	if (_ptrSercom == NULL)
   {
      return false;
   }
	
   bool isrState = disableInterrupt(_ptrSercom);

   //save the enabled state.
   uint32_t bit = _ptrSercom->USART.CTRLA.bit.ENABLE;

   //disable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 0;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //enable parity if needed
   if (UART_PARITY_NONE == parity_bits)
   {
	   _ptrSercom->USART.CTRLA.bit.FORM=0;
   } else 
   {
	   _ptrSercom->USART.CTRLA.bit.FORM=1;
	   _ptrSercom->USART.CTRLB.bit.PMODE = ((uint32_t)parity_bits-1);
   }
   

   //restore the enable bit
   _ptrSercom->USART.CTRLA.bit.ENABLE = bit;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //restore the interrupt state
   restoreInterrupt(_ptrSercom, isrState);
   return true;
}
	
uart_parity_t UART::getParity()
{
	return (uart_parity_t)_ptrSercom->USART.CTRLB.bit.PMODE;
}

bool UART::setDataBits(uint8_t bits)
{
	if (_ptrSercom == NULL)
   {
      return false;
   }
	
   if (bits <5 || bits>9) 
   {
	   ERROR("Wrong bit size: %d", bits);
	   return false;
   }
   bool isrState = disableInterrupt(_ptrSercom);

   //save the enabled state.
   uint32_t bit = _ptrSercom->USART.CTRLA.bit.ENABLE;

   //disable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 0;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   uint32_t x=0;
   if (bits == 9)
   {
	   x=0x01;
   }
   if (bits == 7)
   {
	   x=0x07;
   }
   if (bits == 6)
   {
	   x=0x06;
   }
   if (bits == 5)
   {
	   x=0x05;
   }
   //set data bits
   _ptrSercom->USART.CTRLB.bit.CHSIZE = x;

   //restore the enable bit
   _ptrSercom->USART.CTRLA.bit.ENABLE = bit;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //restore the interrupt state
   restoreInterrupt(_ptrSercom, isrState);
   return true;
}

uint8_t UART::getDataBits()
{
	uint32_t x;
	x=_ptrSercom->USART.CTRLB.bit.CHSIZE;
	if (x==0) 
	{
		return 8;
	}
	if (x==1) 
	{
		return 9;
	}
	return x; 
}

bool UART::setBaud(uint32_t baud)
{
   uint32_t bit;
   _baud = 0;
   if (_ptrSercom == NULL)
   {
      return false;
   }

   bool isrState = disableInterrupt(_ptrSercom);

   //save the enabled state.
   bit = _ptrSercom->USART.CTRLA.bit.ENABLE;

   //disable the UART
   _ptrSercom->USART.CTRLA.bit.ENABLE = 0;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //set baud
   int32_t x = (uint32_t)(((uint64_t)65536UL * (48000000UL - 16 * baud)) / 48000000UL);
   _ptrSercom->USART.BAUD.reg = x;

   //restore the enable bit
   _ptrSercom->USART.CTRLA.bit.ENABLE = bit;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      ERROR("Time out waiting for sync busy");
      return false;
   }

   //restore the interrupt state
   restoreInterrupt(_ptrSercom, isrState);
   _baud = baud;
   return true;
}


//TODO we need to store the line state for 
//  to make sure we reint the same
bool UART::_reset(void)
{
	
   //reset the UART peripheral
   _ptrSercom->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;

   //wait for the peripheral to sync
   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.SWRST == 1; }, 100000))
   {
      _ptrSercom = NULL;  // clear pointer so we know peripheral not setup
      ERROR("Time out waiting for sync busy");
      return false;
   }

   _ptrSercom->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD           //LSB transmitted first
                                 | SERCOM_USART_CTRLA_RXPO(0x01)   //RX pin is pad[1]
                                 | SERCOM_USART_CTRLA_MODE(0x01);  //USART with internal clock

   _ptrSercom->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN          //enable RX
                                 | SERCOM_USART_CTRLB_TXEN        //enable TX
                                 | SERCOM_USART_CTRLB_CHSIZE(0);  //set as 8 bit mode

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.CTRLB == 1; }, 100000))
   {
      _ptrSercom = NULL;  // clear pointer so we know peripheral not setup
      ERROR("Time out waiting for sync busy");
      return false;
   }

   if (!setBaud(_baud))
   {
      _ptrSercom = NULL;  // clear pointer so we know peripheral not setup
      ERROR("Could not set baud");
      return false;
   }

   //setup the CTRLA register
   _ptrSercom->USART.CTRLA.reg = SERCOM_USART_CTRLA_DORD          //LSB transmitted first
                                 | SERCOM_USART_CTRLA_RXPO(0x01)  //RX pin is pad[1]
                                 | SERCOM_USART_CTRLA_MODE(0x01)  //USART with internal clock
                                 | SERCOM_USART_CTRLA_ENABLE;

   if (!waitTimeoutWhile([this](void) { return _ptrSercom->USART.SYNCBUSY.bit.ENABLE == 1; }, 100000))
   {
      _ptrSercom = NULL;  // clear pointer so we know peripheral not setup
      ERROR("Time out waiting for sync busy");
      return false;
   }

   _ptrSercom->USART.INTFLAG.reg = SERCOM_I2CM_INTFLAG_MASK;
   _ptrSercom->USART.INTENCLR.reg = SERCOM_USART_INTENSET_MASK;
   // _ptrSercom->USART.INTENSET.reg=SERCOM_USART_INTENSET_DRE;
   _ptrSercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
   
   _framing_errors=0;
  
   return true;
}

bool UART::setup(const pin_t tx, const pin_t rx, uint32_t baud,uint8_t nvic_priority)
{
   //set up pins for SERCOM0
   _ptrSercom = (Sercom *)tx.ptrPerherial;
   _baud = baud;

   PinSetAsOutput(tx);
   PinSetAsInput(rx);
   PinConfig(tx);
   PinConfig(rx);

   enableSercomNVICAndClocks(_ptrSercom, isrHandler, this,nvic_priority);
   _irq_priority=nvic_priority;
   
   //_txFifo.reset();
   return _reset();
}

size_t UART::read(uint8_t *ptrData, size_t numberBytes)
{
   uint32_t i, n;
   if (_ptrSercom == NULL)
   {
      return 0;
   }
   

   _ptrSercom->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
   n = rxFifo.count();
   _ptrSercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
   if (n > numberBytes)
   {
      n = numberBytes;
   }
   for (i = 0; i < n; i++)
   {
      uint8_t data = 0;

      _ptrSercom->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
      rxFifo.pop(&data);
      _ptrSercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
      ptrData[i] = data;
   }
   return (size_t)n;
}
size_t UART::available(void)
{
	
   size_t n;
   if (_ptrSercom == NULL)
   {
      return 0;
   }
   

   _ptrSercom->USART.INTENCLR.reg = SERCOM_USART_INTENSET_RXC;
   n = (size_t)rxFifo.count();
   _ptrSercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
   return n;
}

size_t UART::write(const uint8_t *ptrData, size_t numberBytes)
{
   volatile uint32_t n=0, x=0;

   if (_ptrSercom == NULL)
   {
      return 0;
   }

   while (numberBytes > 0)
   {
      while (_in_progress)
      {
      }

      //if we call this function from outside and ISR and then from
      // inside an ISR we could have a buffer overflow.
      // so we turn off all ISRs during function call.
      bool isr_state = InterruptDisable();
      n = (UART_FIFO_SIZE_TX - _tx_buf_index);
      if (numberBytes < n)
      {
         n = numberBytes;
      }

#ifdef DEBUG
      if ((_tx_buf_index + n) > UART_FIFO_SIZE_TX)
      {
         __BKPT(3);  //you are overwritting buffer size
      }
#endif

      memcpy(&_tx_buff[_tx_buf_num][_tx_buf_index], ptrData, n);
      _tx_buf_index += n;

      InterruptEnable(isr_state);
      
      _dma_write(); //start DMA transfer of the data. 
      
      x = x + n;
      numberBytes -= n;
   }

   return x;
}
