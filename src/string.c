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
		if (!c1)
			break;
		count--;
	}
	return 0;
}