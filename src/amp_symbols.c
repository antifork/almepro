/* $Id$ */
/*
 * almepro -- symbols database
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


#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <amp.h>
#include <global.h>

TAILQ_HEAD(, symb_entry) symb_head;

	struct symb_entry {
		char *name;	/* symbolic name */
		u_long value;	/* absolute address */
		u_long flags;	/* BFD symbols' flags */
		       TAILQ_ENTRY(symb_entry) entries;
	};


	void add_symbol(char *, u_long, u_long) __attribute__((weak));
	void
	     add_symbol(nm, vl, fl)
	char *nm;
	u_long vl;
	u_long fl;
{
	struct symb_entry *new, *np;

	/* create a new symb_entry */

	new = exec(malloc, sizeof(struct symb_entry));
	new->name = strdup(nm);
	new->value = vl;
	new->flags = fl;

	/*
	 * push the new entry into the sorted queue.
         * we traverse the queue, until the new value is greater than the queue's element.
	 */

	for (np = symb_head.tqh_first; np != NULL && np->value < vl; np = np->entries.tqe_next)
		continue;

	if (np == NULL) {
		if (TAILQ_FIRST(&symb_head) == NULL)
			TAILQ_INSERT_HEAD(&symb_head, new, entries);
		else
			TAILQ_INSERT_TAIL(&symb_head, new, entries);

		return;
	}
	TAILQ_INSERT_BEFORE(np, new, entries);
	return;
}


/*
 * given an address, return a description like entry_point+offset
 */

static char buffer[80];

char *get_symb_descr(u_long) __attribute__((weak));
char *
get_symb_descr(addr)
	u_long addr;
{
	char offset[12];
	struct symb_entry *np;

	if (__options.stripped)
		return ("stripped");

	/*
	 *  searching for the nearest entrypoint
	 */

	for (np = symb_head.tqh_first;
	     (np != NULL) && (np->entries.tqe_next != NULL) && (addr >= np->entries.tqe_next->value); np = np->entries.tqe_next)
		continue;

	if ((np == NULL) || (np->entries.tqe_next == NULL))
		return ("__??");/* XXX: debug */


	strlcpy(buffer, np->name, sizeof(buffer));

	if (addr - np->value > 0) {
		ksprintf(offset, "%d", addr - np->value);
		strlcat(buffer, "+", sizeof(buffer));
		strlcat(buffer, offset, sizeof(buffer));
	}
	return (buffer);
}


/*
 * print the whole symbol queue. (debug)
 */

void dump_symbols() __attribute__((weak));
void
dump_symbols()
{
	struct symb_entry *np;

	TAILQ_FOREACH(np, &symb_head, entries) {
		PUTS(~, "%s %p\n", np->name, np->value);
	}

}
