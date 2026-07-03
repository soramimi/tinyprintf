
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
			int width = 0;
			char sign = 0;
			char padding = 0;
			while (right < end) {
				c = (unsigned char)*right++;
				if (c == 'd') {
					int v = va_arg(args, int);
					int n = 0;
					char tmp[16];
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
					
					char pad = padding ? padding : ' ';
					
					if (sign != 0 && pad != '0') { *--q = sign; n++; }
					
					int w = sizeof(tmp);
					w = w < width ? w : width;
					if (sign != 0 && w > 0) {
						w--;
					}
					
					while (n < w) {
						*--q = pad;
						n++;
					}
					
					if (sign != 0 && pad == '0') { *--q = sign; n++; }
					
					ret += x_write(q, n);
					break;
				}
				if (c == 'x' || c == 'X') {
					unsigned int u = va_arg(args, unsigned int);
					int n = 0;
					char tmp[16];
					char *q = tmp + sizeof(tmp);
					do {
						char d = u & 0xf;
						d = d < 10 ? ('0' + d) : ((c == 'x' ? 'a' : 'A') + (d - 10));
						*--q = d;
						u >>= 4;
						n++;
					} while (u > 0);
					
					char pad = padding ? padding : ' ';
					
					int w = sizeof(tmp);
					w = w < width ? w : width;
					while (n < w) {
						*--q = pad;
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
				if (c == ' ') {
					padding = ' ';
				} else if (isdigit(c)) {
					if (padding == 0 && c == '0') {
						padding = c;
					}
					width = width * 10 + (c - '0');
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
	x_printf("Hello,%08x %s%c\n", 0x1234abcd, "world", '!');
	return 0;
}
