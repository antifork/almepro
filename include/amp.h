/*
 * almepro -- allocation memory profiler library
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
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

#ifndef AMP_H
#define AMP_H

#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <config.h>
#include <limits.h>
#include <assert.h>

/*
 *  BSD queue issue.
 */

#if defined(HAVE_BSD_SYS_QUEUE_H)
#include <sys/queue.h>
#else
#include <missing/sys/queue.h>
#endif

/*
 * Macros
 */

#ifndef MIN
#define MIN(a,b) ((a)<(b) ? (a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b) ? (a):(b))
#endif
#define PUTS(x,f,args...)  kprintf(#x " " f,##args)

#define x86_CALL    5 /* bytes */

#define get_caller_addr() __builtin_return_address(0)-x86_CALL

#define _DLBUF_SIZE    	1<<16	/* 64k bytes */	

#if defined(__OpenBSD__)
#define _DLMODE         DL_LAZY
#define _DLSYMPREFIX    "_"
#elif defined(__FreeBSD__)
#define _DLMODE         RTLD_NOW
#define _DLSYMPREFIX    ""
#elif defined(linux)
#define _DLMODE         RTLD_NOW | RTLD_GLOBAL
#define _DLSYMPREFIX    ""
#elif defined(__NetBSD__)
#define _DLMODE         RTLD_NOW
#define _DLSYMPREFIX    ""
#elif
#error "unsupported system"
#endif

/*
 *  Macro 
 */

#define DL_ISLOCKED(x)      ( __sem.x > 0)
#define DL_LOCK(x)       do { __sem.x++; } while (0)
#define DL_UNLOCK(x)     do { __sem.x--; } while (0)

#define exec(x,...)  ({                                                 \
	assert (__libc_so.x != NULL);					\
        __libc_so.x(__VA_ARGS__);                                       \
})

#define DL_ALLOC(x) ({							\
void *ret;								\
ret = dl_buf + dl_offs;							\
dl_offs += x;								\
assert(dl_offs < _DLBUF_SIZE);						\
ret;									\
})

#if !defined (RTLD_NEXT)
#define DL_OPEN(x)	do {						\
	if (__handler == 0)						\
		__handler = dl_open(x,_DLMODE);				\
} while (0)
#else
/* dlopen is not required */
#define DL_OPEN(x)	do { } while (0)
#endif /* DL_OPEN */

#if defined (RTLD_NEXT)
/* #1 : using RTLD_NEXT handler if supported (without dlopen)*/ 
#define DL_SYM(x)	do {						\
	if (__libc_so.x == 0) {                                         \
	__libc_so.x = dlsym (RTLD_NEXT, _DLSYMPREFIX #x);   		\
	}                                                               \
} while (0)
#else
/* #2 : using the opened __handler */
#define DL_SYM(x)	do {						\
    if (__libc_so.x == 0) {                                             \
        assert(__handler != NULL);                                     	\
        __libc_so.x = dlsym (__handler, _DLSYMPREFIX #x);  		\
    }                                                                   \
    assert(__libc_so.x != NULL);                                        \
} while (0)
#endif

/* prototypes */

#if 0
#ifdef HAVE___LIBC_MALLOC
extern void *(*__libc_calloc)(size_t, size_t);
extern void *(*__libc_malloc)(size_t);
extern void (*__libc_free)(void *);
extern void *(*__libc_realloc)(void *, size_t);
extern int (*__libc_sigaction)(int, const struct sigaction *, struct sigaction *);
#endif
#endif /* 0 */

#define INBOUND(a,x,b)	( (u_long)a <= (u_long)x && (u_long)x <= (u_long)b )

/*
 * struct definition
 */

struct OPT {
        int main:1;
        int symbol:1;
        int stripped:1;
        int noname:1;
        int header:1;
};

struct STDLIB {
        void *(*calloc)();      	/* alloc */
        void *(*malloc)();
        void *(*realloc)();
        void  (*free)();

        char *(*strdup)();           	/* string */

        int  (*vasprintf)();        	/* printf */
        int  (*asprintf)();

        RETSIGTYPE (*signal)();    	/* signal */
        int  (*sigaction)();
};

struct GLOBAL {
        long    mem;                    

        u_long  symb_low;
        u_long  symb_high;

        long    malloc;
        long    calloc;
        long    realloc;
        long    free;
	long	strdup;

        long    asprintf;
        long    vasprintf;
        long    signal;
	long    sigaction;
};

struct SEM {
        long    malloc;
        long    calloc;
        long    realloc;
        long    free;
        long    strdup;

        long    asprintf;
        long    vasprintf;
        long    signal;
        long    sigaction;
};

#define T_NULL			0
#define T_MALLOC		1
#define T_CALLOC		2
#define T_REALLOC		3
#define T_STRDUP		4
#define T_ASPRINTF		5	
#define T_VASPRINTF		6	
#define T_FREE			7	
#define T_REGFREE		8	
#define T_EXIT			9	

#endif				/* AMP_H */
