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

#ifndef NANODMA_H_
#define NANODMA_H_
#include "sam.h"
#include <memory.h>

#define NANO_DMA_NUM_CHANNELS (5)

typedef enum  {
	/** 8-bit access. */
	DMA_BEAT_SIZE_BYTE = 0,
	/** 16-bit access. */
	DMA_BEAT_SIZE_HWORD =1,
	/** 32-bit access. */
	DMA_BEAT_SIZE_WORD = 2,
} NanoDMABeatSize_t;

typedef enum {
	NANO_DMA_TRANSFER_ERROR = 0x01,
	NANO_DMA_TRANSFER_COMPLETE = 0x02,
	NANO_DMA_TRANSFER_SUSPEND =0x04
}NanoDMAInterruptFlag_t;

typedef enum {
	NANO_DMA_STATUS_IDLE = 0,
	NANO_DMA_STATUS_BUSY = 1,
	NANO_DMA_STATUS_ERROR =2,
	NANO_DMA_STATUS_DONE =3,
	NANO_DMA_STATUS_SUSPEND =4,
}NanoDMAStatus_t;

/*
 * 
 * _tx_dma.config(4, getSercomDMA_TX_ID(_ptrSercom), DMAC_CHCTRLA_TRIGACT_BURST_Val, 0, false);

	_tx_dma.ptrUserData=(void *) this;
	// _tx_dma.setCallback(txDMADone);


	_tx_dma.setFirstTransfer( _dma_buff, (void *)&_ptrSercom->USART.DATA.reg, 
			n, DMA_BEAT_SIZE_BYTE,
			true,false, NULL);

	__DSB();
	__ISB();
	_tx_dma.startTransfer(0); //start with highest interrupt priority. 
 * 
 */

typedef void (*NanoDMAcallback_t)(void *ptrClass, NanoDMAInterruptFlag_t flag);
class NanoDMA {
	private:
		  static __attribute__ ((aligned (8))) DmacDescriptor _descriptor[NANO_DMA_NUM_CHANNELS];
		  static __attribute__ ((aligned (8))) DmacDescriptor _write_back_section[NANO_DMA_NUM_CHANNELS];

		  volatile static bool _perpherialInitDone;
		  static volatile uint16_t _usedChannels; //bit mask for channels that have been used

		  volatile uint8_t _channel;
		  NanoDMAcallback_t _callback;
		  volatile NanoDMAStatus_t _status;

		  DmacDescriptor *_ptrLastDescriptorTransfered;  //this is a pointer to the last descriptor DMA'd

		  uint8_t findNextFreeChannel(void); //find the next channel that is free


	public:
		  void *ptrUserData;
		  friend void DMAC_Handler( void );
		  static NanoDMA * ptrDMAS[NANO_DMA_NUM_CHANNELS];
		  NanoDMA(void);
		  ~NanoDMA(void) {};
		  bool setup(void);
		  bool release(void);
		  NanoDMAcallback_t getCallback(void);
		  DmacDescriptor *getFirstDescriptor(void);
		  DmacDescriptor *findDescriptor(DmacDescriptor *nextDescriptorMatch);
		  bool config(uint8_t priority, uint8_t perpherialTrigger,
				  uint8_t triggerAction, uint8_t eventInputAction,
				   bool eventOutputAction);

		  bool setFirstTransfer(void *source_memory, void *destination_memory,
					  uint32_t xfercount, NanoDMABeatSize_t beatsize,
					  bool srcinc, bool destinc, DmacDescriptor *nextDescriptor=NULL);

		  bool configDescriptor(DmacDescriptor *buffer, void *source_memory, void *destination_memory,
					  uint32_t xfercount, NanoDMABeatSize_t beatsize,
					  bool srcinc, bool destinc, DmacDescriptor *nextDescriptor=NULL);

		  DmacDescriptor *getLastDescriptorTransfered(void) { return _ptrLastDescriptorTransfered;}

		  bool enableDescriptor(DmacDescriptor *ptrDesc);

		  bool setCallback(NanoDMAcallback_t callback);
		  bool startTransfer(uint32_t isr_priority=2);
		  bool stopTransfer(void);
		  void setStatus(NanoDMAStatus_t status);
		  NanoDMAStatus_t getStatus(void) {return _status;}
		  bool isBusy(void);
		  void printPostMortem(void);

		  void *getSourceStartingAddress(DmacDescriptor * ptrDesc);
		  void *getDestStartingAddress(DmacDescriptor * ptrDesc);
		  uint8_t *getActiveSourceBuffer(void);
};





#endif /* NANODMA_H_ */
