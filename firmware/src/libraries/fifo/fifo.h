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
#ifndef _LIBRARIES_FIFO_FIFO_H_
#define _LIBRARIES_FIFO_FIFO_H_

#include <stdint.h>
#include <memory.h>

//typedef void (*functionPtr)(void);

template <class T, uint32_t N>
class FIFO {
    public:
	FIFO();

	bool push(const T *ptrData);
	bool pop(T *ptrData);

	uint32_t count(void); //number of packets in buffer.
	bool empty(void);
	bool full(void);
	void reset(void);

    private:
	T _data[N];
	volatile uint32_t _readIndex;
	volatile uint32_t _writeIndex;
	volatile uint32_t _count;
};

template <class T, uint32_t N>
FIFO<T,N>::FIFO()
{
	_readIndex=0;
	_writeIndex=0;
	_count=0;
}

template <class T, uint32_t N>
void  FIFO<T,N>::reset(void)
{
	_readIndex=0;
	_writeIndex=0;
	_count=0;
}


template <class T, uint32_t N>
bool  FIFO<T,N>::push(const T *ptrData)
{
	if (full())
	{
	    return false;
	}

	memcpy(&_data[_writeIndex],ptrData,sizeof(T));
	_writeIndex++;
	if (_writeIndex>=N)
	{
	    _writeIndex=0;
	}
	_count++;

	return true;
}


template <class T, uint32_t N>
bool  FIFO<T,N>::pop(T *ptrData)
{
	if (empty())
	{
	    return false;
	}

	memcpy(ptrData,&_data[_readIndex],sizeof(T));
	_readIndex++;
	if (_readIndex>=N)
	{
	    _readIndex=0;
	}
	_count--;
	
	return true;
}

template <class T, uint32_t N>
uint32_t  FIFO<T,N>::count(void) //number of packets in buffer.
{
	return _count;
}


template <class T, uint32_t N>
bool FIFO<T,N>::empty(void)
{
	if (0 == count())
	{
	    return true;
	}
	return false;
}

template <class T, uint32_t N>
bool FIFO<T,N>::full(void)
{
	if (N == count())
	{
	    return true;
	}
	return false;
}





#endif /* _LIBRARIES_FIFO_FIFO_H_ */
