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
#ifndef SRC_DRIVERS_GCLK_GCLK_H_
#define SRC_DRIVERS_GCLK_GCLK_H_

#include <stdint.h>



bool glck_enable_clock(uint32_t clock_source, uint32_t id);


#endif /* SRC_DRIVERS_GCLK_GCLK_H_ */
