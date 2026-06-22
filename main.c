
// tiny printf for micro controllers

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

int x_write(char const *p, int n)
{
	fwrite(p, 1, n, stdout);
	return n;
}

int x_printf(char const *fmt, ...)
{
	int ret = 0;
	va_list args;
	va_start(args, fmt);
	char const *end = fmt + strlen(fmt);
	char const *left = fmt;
	char const *right = fmt;
	while (1) {
		int c = 0;
		if (right < end) {
			c = (unsigned char)*right;
		}
		if (c == '%' || c == 0) {
			if (left < right) {
				ret += x_write(left, right - left);
				left = right;
			}
			if (c == 0) break;
			right++;
			char sign = 0;
			while (right < end) {
				c = (unsigned char)*right++;
				if (c == 'd') {
					int v = va_arg(args, int);
					int n = 0;
					char tmp[12];
					char *q = tmp + sizeof(tmp);
					unsigned int u;
					if (v < 0) {
						sign = '-';
						u = -v;
					} else {
						u = v;
					}
					do {
						*--q = '0' + (u % 10);
						u /= 10;
						n++;
					} while (u > 0);
					if (sign != 0) {
						*--q = sign;
						n++;
					}
					ret += x_write(q, n);
					break;
				}
				if (c == 's') {
					char *s = va_arg(args, char *);
					int n = strlen(s);
					ret += x_write(s, n);
					break;
				}
				if (c == 'c') {
					char ch = (char)va_arg(args, int);
					ret += x_write(&ch, 1);
					break;
				}
				if (isalpha(c)) {
					ret += x_write(left, right - left);
					break;
				}
				if (c == '+') {
					sign = c;
				}
			}
			left = right;
		} else {
			right++;
		}
	}
	va_end(args);
	return ret;
}

int main()
{
	x_printf("Hello,%d %s%c\n", INT_MAX, "world", '!');
	return 0;
}
