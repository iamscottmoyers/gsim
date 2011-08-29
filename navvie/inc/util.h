#ifndef _NV_UTIL_H_
#define _NV_UTIL_H_

void nv_str_toupper(char *str);
void nv_fix_new_lines(char *str);
#define OFFSET_OF( _type, _member ) ( (size_t) &((_type *) 0)->_member )
#define CONTAINER_OF( _ptr, _type, _member ) ((_type *)((char *)_ptr - OFFSET_OF(_type, _member)))

#endif
