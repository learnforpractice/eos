#include <stdio.h>
#include <stdarg.h>
void prints(const char *str);
int fprintf(FILE *restrict f, const char *restrict fmt, ...)
{
//	prints(fmt);
//	return 0;
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = vfprintf(f, fmt, ap);
	va_end(ap);
	return ret;
}
