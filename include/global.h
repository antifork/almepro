/*
 * almepro -- globals  
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

#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef 	GLOBAL_INTERNAL 
#define EXTERN
#define INIT(x...) x
#else
#define EXTERN extern
#define INIT(x...)
#endif

#include <amp.h>

EXTERN char *__label[12] INIT( = {
        [T_NULL] = NULL,
        [T_MALLOC] = "<malloc>   ",
        [T_CALLOC] = "<calloc>   ",
        [T_REALLOC] = "<realloc>  ",
        [T_STRDUP] = "<strdup>   ",
        [T_ASPRINTF] = "<asprintf> ",
        [T_VASPRINTF] = "<vasprintf>",
        [T_FREE] = "<free>     ",
        [T_REGFREE] = "<regfree>  ",
        [T_EXIT] = "ret        ",
});

EXTERN char *__unit[] INIT(= {"b", "K", "M", "G", "T"});

EXTERN struct sigaction __orig[32];
EXTERN struct sigaction __hdr;

EXTERN struct OPT __options;                        
EXTERN struct STDLIB __libc_so;
EXTERN struct GLOBAL __global INIT (= { 
	mem:		0,
	symb_low:	ULONG_MAX,
	symb_high:	0,
	malloc:		0,
	calloc:		0,
	realloc:	0,
	free:		0,
	strdup:		0,
	asprintf:	0,
	vasprintf:	0,
	signal:		0, 
	sigaction:	0,
});

#endif /* GLOBAL_H */
