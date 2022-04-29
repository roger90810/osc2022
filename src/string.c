#include "string.h"
/**
 * @brief Compare two strings
 * 
 * @param cs One string
 * @param ct Another string
 * @return int 
 */
int strcmp(const char *cs, const char *ct)
{
    unsigned char c1, c2;

	while (1) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		if (!c1)
			break;
	}
	return 0;
}

/**
 * @brief Compare two length-limited strings
 * 
 * @param cs One string
 * @param ct Another string
 * @param count The maximum number of bytes to compare
 * @return int 
 */
int strncmp(const char *cs, const char *ct, unsigned int count)
{
	unsigned char c1, c2;

	while (count) {
		c1 = *cs++;
		c2 = *ct++;
		if (c1 != c2)
			return c1 < c2 ? -1 : 1;
		// if (!c1)
		// 	break;
		count--;
	}
	return 0;
}

/**
 * @brief Find the first occurrence of a set of characters
 * 
 * @param cs The string to be searched
 * @param ct The characters to search for
 * @return char* 
 */
char *strpbrk(const char *cs, const char *ct)
{
    const char *sc1, *sc2;

    for (sc1 = cs; *sc1 != '\0'; ++sc1) {
        for (sc2 = ct; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return 0;
}

/**
 * @brief Split a string into tokens, updates s to point after the token, ready for the next call.
 * 
 * @param s The string to be searched
 * @param ct The characters to search for
 * @return char* 
 */
char *strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == 0)
        return 0;

    end = strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
    *s = end;
    return sbegin;
}

/**
 * @brief Find the length of a string
 * 
 * @param s The string to be sized
 * @return uint64_t 
 */
uint64_t strlen(const char *s)
{
	const char *sc;	

	for (sc = s; *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}

uint32_t atou(const char *s)
{
	uint32_t i = 0;
	while (IS_DIGIT(*s))
		i = i * 10 + (*s++ - '0');
	return i;
}

/**
 * @brief Copy a length-limited, C-string
 * The result is not %NUL-terminated if the source exceeds count bytes.
 * In the case where the length of src is less than  that  of
 * count, the remainder of dest will be padded with %NUL.
 * 
 * @param dest Where to copy the string to
 * @param src Where to copy the string from
 * @param count The maximum number of bytes to copy
 * @return char*
 */
char *strncpy(char *dest, const char *src, uint32_t count)
{
	char *tmp = dest;
	while (count) {
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	return dest;
}

/**
 * @brief Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * Do not use memset() to access IO space, use memset_io() instead.
 */
void *memset(void *s, int c, unsigned long count)
{
	char *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}