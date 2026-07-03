
// tiny printf for micro controllers

#include "tinyprintf.h"

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

// Emit a pre-rendered numeric body with optional sign and field width.
// The padding characters are written directly, so the field width is not
// limited by any temporary buffer size.
static int x_emit_num(char const *body, int body_len, char sign, int width, char pad)
{
	int ret = 0;
	int total = body_len + (sign ? 1 : 0);
	int pad_count = width > total ? width - total : 0;
	if (pad == '0') {
		if (sign) ret += x_write(&sign, 1);
		while (pad_count-- > 0) ret += x_write("0", 1);
	} else {
		while (pad_count-- > 0) ret += x_write(" ", 1);
		if (sign) ret += x_write(&sign, 1);
	}
	ret += x_write(body, body_len);
	return ret;
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
			int in_width = 0;
			while (right < end) {
				c = (unsigned char)*right++;
				if (c == 'd') {
					int v = va_arg(args, int);
					unsigned int u;
					if (v < 0) {
						sign = '-';
						u = -(unsigned int)v;
					} else {
						u = v;
					}
					char tmp[20];
					char *q = tmp + sizeof(tmp);
					int n = 0;
					do {
						*--q = '0' + (u % 10);
						u /= 10;
						n++;
					} while (u > 0);
					ret += x_emit_num(q, n, sign, width, padding ? padding : ' ');
					break;
				}
				if (c == 'u') {
					unsigned int u = va_arg(args, unsigned int);
					char tmp[20];
					char *q = tmp + sizeof(tmp);
					int n = 0;
					do {
						*--q = '0' + (u % 10);
						u /= 10;
						n++;
					} while (u > 0);
					ret += x_emit_num(q, n, 0, width, padding ? padding : ' ');
					break;
				}
				if (c == 'x' || c == 'X') {
					unsigned int u = va_arg(args, unsigned int);
					char tmp[20];
					char *q = tmp + sizeof(tmp);
					int n = 0;
					do {
						char d = u & 0xf;
						d = d < 10 ? ('0' + d) : ((c == 'x' ? 'a' : 'A') + (d - 10));
						*--q = d;
						u >>= 4;
						n++;
					} while (u > 0);
					ret += x_emit_num(q, n, 0, width, padding ? padding : ' ');
					break;
				}
				if (c == 's') {
					char *s = va_arg(args, char *);
					if (!s) {
						s = "(null)";
					}
					ret += x_write(s, strlen(s));
					break;
				}
				if (c == 'c') {
					char ch = (char)va_arg(args, int);
					ret += x_write(&ch, 1);
					break;
				}
				if (c == '%') {
					char pc = '%';
					ret += x_write(&pc, 1);
					break;
				}
				if (isalpha(c)) {
					ret += x_write(left, right - left);
					break;
				}
				if (c == '+') {
					sign = '+';
				} else if (c == ' ') {
					padding = ' ';
				} else if (isdigit(c)) {
					if (!in_width && c == '0') {
						padding = '0';
					} else {
						width = width * 10 + (c - '0');
					}
					in_width = 1;
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
