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

#ifndef SRC_DEVICES_CHARDEVICE_H_
#define SRC_DEVICES_CHARDEVICE_H_



#include <inttypes.h>

class CharDevice
{
  public:
    //virtual char getChar(void)=0;
    //virtual void putChar(char c)=0;
    virtual size_t write(const uint8_t *ptrData, size_t numberBytes)=0;
    virtual size_t read(uint8_t *ptrData, size_t numberBytes)=0;
    virtual size_t available(void)=0;
    virtual void flush_tx(void)=0;
};


#endif /* SRC_DEVICES_CHARDEVICE_H_ */
