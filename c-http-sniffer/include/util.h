/* util.h */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <ctype.h>

/************************Macros*****************************/
#define MALLOC(type, num)  (type *) check_malloc((num) * sizeof(type))
#define CALLOC(type, num)  (type *) check_calloc((num) * sizeof(type))

/**********************Functions*****************************/

void *check_malloc(unsigned long size);
void *check_calloc(unsigned long size);
char *ip_ntos(u_int32_t n);
u_int32_t ip_ston(char *s);
void error(char* s);

#endif	/* __UTIL_H__ */
