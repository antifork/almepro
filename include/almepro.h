/*
 * almepro -- allocation memory profiler library
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
 *                    Banchi Leonardo <benkj@antifork.org>
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

#ifndef ALMEPRO_H
#define ALMEPRO_H

/*
 * common headers
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>
#include <regex.h>

/*
 * prototipes
 */

extern void *amp_calloc(char *, int, size_t, size_t);
extern void *amp_malloc(char *, int, size_t);
extern void amp_free(char *, int, void *);
extern void *amp_realloc(char *, int, void *, size_t);
extern char *amp_strdup(char *, int, char *);
extern int amp_asprintf(char *, int, char **, char *,...);
extern int amp_vasprintf(char *, int, char **, char *, va_list);
extern void amp_regfree(char *, int, regex_t *);
extern void amp_exit(char *, int, void (*) (void));
extern void (*amp_signal(char *, int ,int , void (*) (int))) (int);

/*
 * CPP stubs: the following functions are currently tracert by almepro library
 */

#define calloc(p,s) 		amp_calloc   (__FILE__,__LINE__,p,s)
#define malloc(p)		amp_malloc   (__FILE__,__LINE__,p)
#define free(p)			amp_free     (__FILE__,__LINE__,p)
#define regfree(p)		amp_regfree  (__FILE__,__LINE__,p)
#define realloc(p,s)		amp_realloc  (__FILE__,__LINE__,p,s)
#define strdup(s)		amp_strdup   (__FILE__,__LINE__,s)
#define vasprintf(x,y,z)        amp_vasprintf(__FILE__,__LINE__,x,y,z)
#define asprintf(r,f,args...)   amp_asprintf (__FILE__,__LINE__,r,f, ##args)
#define signal(x,y)             amp_signal   (__FILE__,__LINE__,x,y)

#endif				/* ALMERPRO_H */
