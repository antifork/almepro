/* $Id$ */
/*
 * almepro -- chunk database
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amp.h>
#include <global.h>

/* FIXME: indent bug? */
TAILQ_HEAD(chunk_head_, chunk_entry);
struct chunk_head_ chunk_head;

struct chunk_entry {
	u_long c_addr;		/* caller addr */
	u_long f_addr;		/* entry point */
	u_long type;		/* T_? */

	void *b_addr;		/* base pointer */
	size_t size;		/* size of chunk */
	char *src_pos;		/* filename and line */

	     TAILQ_ENTRY(chunk_entry) entries;
};


static void __add_chunk(u_long, u_long, void *, size_t, char *);
static void
__add_chunk(c_addr, type, b_addr, size, src_pos)
	u_long c_addr;		/* caller addr */
	u_long type;		/* T_? */

	void *b_addr;		/* base pointer */
	size_t size;		/* size of chunk */
	char *src_pos;		/* filename and line */
{
	struct chunk_entry *new, *np;

	/* create a new chunk_entry */

	new = exec(malloc, sizeof(struct chunk_entry));
	new->c_addr = c_addr;
	new->type = type;
	new->b_addr = b_addr;
	new->size = size;
	new->src_pos = src_pos;

	/*
         * push the new entry into the sorted queue.
         * we traverse the queue, until the new value is greater than the queue's element.
         */

	for (np = TAILQ_FIRST(&chunk_head); np != NULL && np->b_addr < b_addr; np = TAILQ_NEXT(np, entries))
		continue;

	if (np == NULL) {
		if (TAILQ_FIRST(&chunk_head) == NULL)
			TAILQ_INSERT_HEAD(&chunk_head, new, entries);
		else
			TAILQ_INSERT_TAIL(&chunk_head, new, entries);
		return;
	}
	TAILQ_INSERT_BEFORE(np, new, entries);
	return;
}

static int __del_chunk(void *, int *, char **);
static int
__del_chunk(b_addr, fb, src_pos)
	void *b_addr;
	int *fb;
	char **src_pos;
{
	struct chunk_entry *np;

	*src_pos = "";
	*fb = 0;
	
	TAILQ_FOREACH(np, &chunk_head, entries) {
		if (np->b_addr == b_addr) {
			*fb = np->size;
			*src_pos = np->src_pos;
			/* chunk is already free */
			if (np->type == T_FREE)
				return 1;
			np->type = T_FREE;
			return (0);
		}
	}

	/* unallocated space? */
	return (2);
}

static char buff[80];

static char *__mem_rehash(long, long);
static char *
__mem_rehash(delta, sign)
	long delta;
	long sign;
{
	long mem;
	long uni;

	if (sign > 0)
		__global.mem += delta;
	else
		__global.mem -= delta > 0 ? delta : 0;

	mem = __global.mem;
	uni = 0;

	while (mem >> 10) {
		mem >>= 10;
		uni++;
	}

	ksprintf(buff, "%d", mem);
	strlcat(buff, "_", sizeof(buff));
	strlcat(buff, __unit[uni], sizeof(buff));

	return (buff);
}

#if 0
int sizeof_chunk(void *) __attribute__((weak));;
int
sizeof_chunk(b_addr)
	void *b_addr;
{
	struct chunk_entry *np;
	int fb;

	TAILQ_FOREACH(np, &chunk_head, entries) {
		if (np->b_addr == b_addr) {
			fb = np->size;
			return (fb);
		}
	}

	return (-1);
}
#endif

static char *mk_line(char *, u_long, char *, char *, u_long, int, char *);
static char *
mk_line(src_pos, c_addr, desc, label, b_addr, size, tot)
	char *src_pos;
	u_long c_addr;
	char *desc, *label;
	u_long b_addr;
	int size;
	char *tot;
{
	char *buff;
	size_t len;

	assert((buff = exec(malloc, __global.ws_col * 2 + 1)) != NULL);

	ksnprintf(buff, __global.ws_col - 3, "%s0x%x%s", src_pos, c_addr, desc);
	len = strlen(buff);

	buff[len++] = ':';
	MEMSET(buff, ' ', len, __global.ws_col + 2);
	buff[__global.ws_col + 1] = '`';
	len = (__global.ws_col * 2) - 50 - PUTS_CHAR;
	MEMSET(buff, '-', __global.ws_col + 2, len);

	ksnprintf(buff + len, 50 + PUTS_CHAR, " call %s\t[0x%x]\t%d\t%s", label, b_addr, size, tot);

	return buff;
}

int add_chunk(unsigned long, void *, size_t, int, int) __attribute__((weak));
int
add_chunk(c_addr, b_addr, size, nmemb, type)
	unsigned long c_addr;
	void *b_addr;
	size_t size;
	int nmemb;
	int type;
{
	char *symb_addr = NULL;
	char *src_pos = amp_get_line(c_addr);
	char *b;

	/* add the chunk to queue */
	__add_chunk(c_addr, type, b_addr, size * nmemb, src_pos);
	b = mk_line(src_pos, c_addr, (char *)get_symb_descr(c_addr), __label[type], (u_long)b_addr, size * nmemb, __mem_rehash(size * nmemb, 1));
	PUTS(+, "%s\n", b);
	exec(free, b);
	//PUTS(+, "%s%p <%s>: call %s\t%p[%d] %s\n", src_pos, c_addr, get_symb_descr(c_addr), __label[type], b_addr, size * nmemb, __mem_rehash(size * nmemb, 1));
	return (0);
}

static const char *chunk_error[3] = {
	NULL,
	"chunk is already free",
	"unallocated space?"
};

int del_chunk(unsigned long, void *, int) __attribute__((weak));
int
del_chunk(c_addr, b_addr, type)
	unsigned long c_addr;
	void *b_addr;
	int type;
{
	char *symb_addr = NULL, *src_pos, *b;
	int fb, err;

	/* remove the chunk from database */

	if ((err = __del_chunk(b_addr, &fb, &src_pos)) && b_addr != NULL) {
		PUTS(*, "free(%p): warning, %s\n", b_addr, chunk_error[err]);
	}
	b = mk_line(src_pos, c_addr, (char *)get_symb_descr(c_addr), __label[type], (u_long)b_addr, -fb, __mem_rehash(fb, -1));
	PUTS(-, "%s\n", b);
	exec(free, b);
	//PUTS(-, "%p <%s>: call %s\t%p[%d] %s\n", c_addr, get_symb_descr(c_addr), __label[type], b_addr, -fb, __mem_rehash(fb, -1));
	return (0);
}

void dump_chunks() __attribute__((weak));
void
dump_chunks()
{
	struct chunk_entry *np;
	char *b;

	if (__options.stripped)
		return;
	PUTS(>, "memory-leak summary (%s):\n", __mem_rehash(0, 0));
	PUTS(>, "stat: malloc=%d calloc=%d, realloc=%d, free=%d\n", __global.malloc, __global.calloc, __global.realloc, __global.free);
	PUTS(>, "      strdup=%d, asprintf=%d, vasprintf=%d\n", __global.strdup, __global.asprintf, __global.vasprintf);
	PUTS(>, "      signal=%d, sigaction=%d\n", __global.signal, __global.sigaction);
	TAILQ_FOREACH(np, &chunk_head, entries) {
		if (np->type == T_FREE)
			continue;
		b = mk_line(np->src_pos, np->c_addr, (char *)get_symb_descr(np->c_addr), __label[np->type], (u_long)np->b_addr, np->size, "");
		PUTS(!, "%s\n", b);
		exec(free, b);
		//PUTS(!, "%p <%s>: call %s\t%p[%d]\n", np->c_addr, get_symb_descr(np->c_addr), __label[np->type], np->b_addr, np->size);
	}
}
