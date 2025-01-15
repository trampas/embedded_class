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

#ifndef SRC_LIBRARIES_LIBC_VSNPRINTF_H_
#define SRC_LIBRARIES_LIBC_VSNPRINTF_H_

#include <stdarg.h>
#include <memory.h> //for size_t
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

size_t _ftoa(char *buf, size_t maxLen, double value, uint8_t prec);
size_t _snprintf(char *ptrStr, size_t maxLen, const char *fmt, ...);
size_t _vsnprintf(char *ptrStr, size_t maxLen, const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif


#endif /* SRC_LIBRARIES_LIBC_VSNPRINTF_H_ */
