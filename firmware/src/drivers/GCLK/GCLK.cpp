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

#include "GCLK.h"
#include "sam.h"
#include "drivers/delay/delay.h"
#include "libraries/syslog/syslog.h"

bool glck_sync(void)
{
	uint32_t t0 = 10000;
		while ( GCLK->SYNCBUSY.reg && t0 > 0)
		{
			/* Wait for synchronization */
			t0--;
			delay_us(1);
		}
		if (t0 == 0)
		{
			ERROR("Could not sync");
			return false;
		}
		return true;
}

bool glck_enable_clock(uint32_t clock_source, uint32_t id)
{
	GCLK->PCHCTRL[id].reg= GCLK_PCHCTRL_CHEN
			| GCLK_PCHCTRL_GEN(clock_source);
	
	return glck_sync();
}


