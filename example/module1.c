#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>

void
sigfun(int a)
{

}
int
main()
{
	int i;
	char *a, *b, *c, *d;

	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, sigfun);

	a = (char *) malloc(512);
	b = (char *) calloc(1024, 2);
	c = (char *) realloc(b, 12);
	c = (char *) realloc(c, 500);
	d = strdup(c);

	free(d);
	free(c);

	for (i = 0; i < 5; i++) {
		a = (char *) malloc(100);
		free(a);
	}

	asprintf(&d, "%d %d", 1000, 10);
	test_val("%d", 100);

	printf("press enter to exit\n");
	getchar();

	exit(1);
}
