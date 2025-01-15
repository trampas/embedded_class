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

#ifndef SRC_DEVICES_BLOCKDEVICE_H_
#define SRC_DEVICES_BLOCKDEVICE_H_

#include <inttypes.h>

class BlockDevice
{
  public:
    virtual size_t write(size_t address, const uint8_t *ptrData, size_t numberBytes)=0;
    virtual size_t read(size_t address, uint8_t *ptrData, size_t numberBytes)=0;
};



#endif /* SRC_DEVICES_BLOCKDEVICE_H_ */
