
// tiny printf for micro controllers -- demo / stdout backend

#include "tinyprintf.h"

#include <stdio.h>

int x_write(char const *p, int n)
{
	fwrite(p, 1, n, stdout);
	return n;
}

int main()
{
	x_printf("Hello,%08x %s%c\n", 0x1234abcd, "world", '!');
	return 0;
}
