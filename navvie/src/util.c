#include <ctype.h>
#include <stdlib.h>

#include "util.h"

void nv_str_toupper(char *str)
{
	if (str != NULL) {
		while(*str != '\0') {
			*str = toupper(*str);
			str++;
		}
	}
}

void nv_fix_new_lines(char *str)
{
	if (str != NULL) {
		while(*str != '\0') {
			if (*str == '\r') {
				*str = '\n';
			}
			str++;
		}
	}
}
