#ifndef TINYPRINTF_H
#define TINYPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

// Platform-provided output sink: write n bytes from p, return bytes written.
// Implement this for your target (UART, stdout, buffer, ...).
int x_write(char const *p, int n);

// Formatted output. Supports: %d %u %x %X %s %c %%
// Flags/width: leading '0' for zero padding, '+' for sign, decimal field width.
int x_printf(char const *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
