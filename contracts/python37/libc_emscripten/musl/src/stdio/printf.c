#include <stdio.h>
#include <stdarg.h>

void prints(const char *str);
int printf(const char *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return ret;
#if 0   
   char output[256];
   va_list args;
   va_start(args, fmt);
   int len = vsnprintf(output, sizeof output, fmt, args);
   va_end(args);
   prints(output);
#endif
}
