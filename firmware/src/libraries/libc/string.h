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

#ifndef SRC_LIBRARIES_LIBC_STRING_H_
#define SRC_LIBRARIES_LIBC_STRING_H_


#include <stdarg.h>
#include <memory.h> //for size_t
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

size_t get_string_after(const char *haystack, const char *needle, char *output, size_t max_length);
int stricmp(char const *a, char const *b);
char* stristr( const char* str1, const char* str2 );
size_t trim_left(char *str,const char *tokens);
size_t trim_right(char *str,const char *tokens);
size_t lstrip(char *str);
size_t rstrip(char *str);
size_t strip(char *str); //removes white space on left and right


#ifdef __cplusplus
}
#endif


#endif /* SRC_LIBRARIES_LIBC_STRING_H_ */
