#ifndef __STRING_H
#define __STRING_H

#include "types.h"

#define IS_DIGIT(c)  ((c>='0')&&(c<='9'))
#define IS_ALPHA(c)  (((c>='a')&&(c<='z'))||((c>='A')&&(c<='Z')))
#define IS_PRINTABLE(c)  ((c>=32)&&(c<=127))
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, unsigned int);
char *strpbrk(const char *, const char *);
char *strsep(char **, const char *);
uint32_t atou(const char *s);
char *strncpy(char *dest, const char *src, uint32_t count);
void *memset(void *s, int c, unsigned long count);
#endif