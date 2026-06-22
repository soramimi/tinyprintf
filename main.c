#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

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
			while (right < end) {
				c = (unsigned char)*right++;
				if (c == 'd') {
					int v = va_arg(args, int);
					if (v == 0) {
						char d = '0';
						ret += x_write(&d, 1);
					} else {
						int n = 0;
						char tmp[100];
						char *q = tmp + sizeof(tmp);
						int neg = 0;
						if (v < 0) {
							neg = 1;
							v = -v;
						}
						while (v > 0) {
							*--q = '0' + (v % 10);
							v /= 10;
							n++;
						}
						if (neg) {
							*--q = '-';
							n++;
						}
						ret += x_write(q, n);
					}
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
	x_printf("Hello,%d %s%c\n", 42, "world", '!');
	return 0;
}
