#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


int 
test_val(char *p,...)
{
	char buff[10];
	char *c;

	va_list ap;
	char *b = NULL;

	va_start(ap, p);
	vasprintf(&b, p, ap);
	va_end(ap);

	free(b);
	c = (char *) malloc(100);

	return;
}
