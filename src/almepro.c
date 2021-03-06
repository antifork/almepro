/* $Id$ */
/*
 * almepro -- allocation memory profiler library
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@antifork.org>
 * 		      Banchi Leonardo <benkj@antifork.org>
 *
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer. 2.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <sys/utsname.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <dlfcn.h>

#include <stdlib.h>
#include <stdarg.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#include <signal.h>
#include <sys/ioctl.h>
#include <config.h>
#include <sensor.h>
#include <amp.h>
#define GLOBAL_INTERNAL
#include <global.h>

/* static buffers */

static void *__handler;
static struct SEM __sem;
static char dl_buf[_DLBUF_SIZE];
static size_t dl_offs;
static u_long saved_symb_high;
static u_long saved_symb_low;

static char *__libs[] = {
        "libc.so.28.3",                	/* openbsd */
        "libc.so.4",                   	/* freebsd */
        "libc.so.6",  			/* linux glibc 2*/
	"libc.so.12",			/* netbsd */
        NULL,
};

RETSIGTYPE
handler(int n)
{
	TRACE_DISABLE();
	dump_chunks();
	TRACE_RESTORE();
}

void *dl_open(char **, int ) __attribute__((weak));
void *
dl_open(libs,flag)
	char **libs;
	int flag;
{
	void *handler=NULL;
	int i=0;

	while ( (handler = dlopen(libs[i],flag)) == NULL ) {
			if ( libs[++i] == NULL ) break;
		}

	return (handler);
}

static void
parse_options()
{
	char *op;

	/* options set by runamp script */
	assert(op = getenv("__AMP_OPTIONS"));

	for ( ; *op != '\0'; op++)
		switch (*op) {
		case 'N' :
			__options.src_path = OSRC_NONE;
			break;
		case 'F' :
			__options.src_path = OSRC_FULL;
			break;
		case 'B' :
			__options.src_path = OSRC_BASE;
			break;
		case 'R' :
			__options.src_path = OSRC_REL;
			break;
		case 'S' :
			__options.no_symbol = 0;
			break;
		case 's' :
			__options.no_symbol = 1;
			break;
		case 'A' :
			__options.trace_all = 0;
			break;
		case 'a' :
			__options.trace_all = 1;
			break;
		case 'o' :
			__options.outfile = getenv("__AMP_OUTFILE");
			break;
		default: 
			break;
		}
}

static void 
library_init()
{
	struct utsname name;

	DL_OPEN(__libs); /* open libc if required */

	/* load symbol needed in the following init functions */
	DL_SYM(malloc);
	DL_SYM(sigaction);
	DL_SYM(strdup);
	DL_SYM(vasprintf);
	DL_SYM(asprintf);
        DL_SYM(signal);

	parse_options();

	/* set output file */
	if (!INIT_STDOUT()) 
		PUTS(*, "can't open output file for writing... using stderr\n");
	
	/* get terminal columns */
	INIT_WINSZ();

	PUTS(>, "almepro-library %s --- please report bugs to <%s>\n", VERSION, PACKAGE_BUGREPORT);

	uname(&name);
	PUTS(>, "uname(sysname=\"%s\",nodename=\"%s\",release=\"%s\",version=\"%s\",machine=\"%s\")\n",
	     name.sysname, name.nodename, name.release, name.version, name.machine);

	if (amp_symbols_init() == -1)
		PUTS(*, "bfd: symbols resolution suppressed\n");

	PUTS(>, "bfd: low_symbol@ %p, high_symbol@ %p\n", __global.symb_low, __global.symb_high);

}

/**********************************************
 *      library constructor/destructor
 **********************************************/

static void init_lib(void) __attribute__((constructor));
static void
init_lib()
{
#ifdef RTLD_NEXT
	library_init();

	/* init signal */
	exec(signal, SIGQUIT, handler);
#else
	__amp_signal(SIGQUIT, handler);
#endif

	amp_sensor();
}

static void exit_lib(void) __attribute__((destructor));
static void
exit_lib()
{
	TRACE_DISABLE();
	
	dump_chunks();
	PUTS(-, "exit (bye)\n");
	FLUSH_OUTPUT();
}


/**********************************************
 *               Public stubs
 **********************************************/

void *
malloc(size)
	size_t size;
{
	void *add;
	void *res;

	if (DL_ISLOCKED(malloc))
		return exist(malloc) ?
			exec(malloc, size) : DL_ALLOC(size);

	DL_LOCK(malloc);

		DL_OPEN(__libs); /* open libc if needed */
		DL_SYM(malloc);  /* resolve symbol if needed */ 

		add = get_caller_addr();
		res = exec(malloc, size); /* call the real libc function */

		if (!TRACE_ALLOWED(add))
			goto malloc_end;

		/* log the chunk */

		__global.malloc++;
		add_chunk(add, res, size, 1, T_MALLOC);

malloc_end:
	DL_UNLOCK(malloc);
	return (res);
}


void
free(ptr)
        void *ptr;
{
        void *add;

        if (DL_ISLOCKED(free))
		return;

        DL_LOCK(free);

                DL_OPEN(__libs); /* open libc if needed */
                DL_SYM(free);  /* resolve symbol if needed */

        	add = get_caller_addr();
		exec(free, ptr);

        	if (!TRACE_ALLOWED(add))
			goto free_end;

               	del_chunk(add, ptr, T_FREE);
        	__global.free++;

free_end:
	DL_UNLOCK(free);
	return;
}

void *
realloc(ptr, size)
	void *ptr;
	size_t size;
{
	void *add;
	void *res;

        if (DL_ISLOCKED(realloc))
                return exist(malloc) ?
                        exec(malloc, size) : DL_ALLOC(size);

        DL_LOCK(realloc);

                DL_OPEN(__libs); /* open libc if needed */
                DL_SYM(realloc);  /* resolve symbol if needed */

		add = get_caller_addr();
		res = exec(realloc, ptr, size);

		if (!TRACE_ALLOWED(add))
			goto realloc_end;	

		__global.realloc++;
		del_chunk(add, ptr, T_REALLOC);
		add_chunk(add, res, size, 1, T_REALLOC);

realloc_end:
        DL_UNLOCK(realloc);
        return (res);
}

void *
calloc(nmemb, size)
	size_t nmemb;
	size_t size;
{
	void *add;
	void *res;

        if (DL_ISLOCKED(calloc))
                return exist(calloc) ?
                        exec(calloc, nmemb, size) : DL_ALLOC(nmemb * size);

        DL_LOCK(calloc);

                DL_OPEN(__libs); /* open libc if needed */
                DL_SYM(calloc);  /* resolve symbol if needed */

                add = get_caller_addr();
                res = exec(calloc, nmemb ,size); /* call the real libc function */

                if (!TRACE_ALLOWED(add))
                        goto calloc_end;

                /* log the chunk */

                __global.calloc++;
		add_chunk(add, res, size, nmemb, T_CALLOC);

calloc_end:
        DL_UNLOCK(calloc);
        return (res);
}

char *
strdup(p)
	const char *p;
{
	void *add;
	char *res;

        if (DL_ISLOCKED(strdup))
		return NULL; 	/* debug purpose only */

        DL_LOCK(free);

                DL_OPEN(__libs); /* open libc if needed */
                DL_SYM(strdup);  /* resolve symbol if needed */

		add = get_caller_addr();
		res = exec(strdup, p);

		if (!TRACE_ALLOWED(add))
			goto free_end;	

		__global.strdup++;
		add_chunk(add, res, strlen(p) + 1, 1, T_STRDUP);

free_end:
	DL_UNLOCK(free);
	return (res);
}

#ifdef HAVE_VASPRINTF
int
vasprintf(ret, format, ap)
	char **ret;
	const char *format;
	va_list ap;
{
	void *add;
	register int i;

        DL_OPEN(__libs); /* open libc if needed */
        DL_SYM(vasprintf);  /* resolve symbol if needed */

	add = get_caller_addr();

	if ((i = exec(vasprintf, ret, format, ap)) != -1 && TRACE_ALLOWED(add))
		add_chunk(add, *ret, i + 1, 1, T_VASPRINTF);

	__global.vasprintf++;
	return (i);
}
#endif

#ifdef HAVE_ASPRINTF
int
asprintf(char **ret, const char *format,...)
{
	register int i;
	void *add;
	va_list ap;


        DL_OPEN(__libs); /* open libc if needed */
        DL_SYM(asprintf);  /* resolve symbol if needed */

	add = get_caller_addr();

	va_start(ap, format);
	if ((i = exec(vasprintf, ret, format, ap)) != -1 && TRACE_ALLOWED(add))
		add_chunk(add, *ret, i + 1, 1, T_ASPRINTF);
	va_end(ap);

	__global.asprintf++;
	return (i);
}
#endif

sig_t
signal(int signum, sig_t handler)
{
	DL_LOCK(signal);

                DL_OPEN(__libs); /* open libc if needed */
		DL_SYM(signal);

		__orig[signum & 31].sa_handler = handler;
		dump_signal(signum, handler, get_caller_addr());
		__global.signal++;

	DL_UNLOCK(signal);

	return (handler);
}

int
sigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
{
	int res;

	DL_LOCK(sigaction);

                DL_OPEN(__libs); /* open libc if needed */
		DL_SYM(sigaction);
		__global.sigaction++;
		res = exec(sigaction, signum, act, oldact);

	DL_UNLOCK(sigaction);

	return (res);
}
