/* $Id$ */
/*
 * almepro -- Binary File Descriptor Library
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

#include <bfd.h>
#include <stdlib.h>
#include <stdio.h>
#include <amp.h>
#include <global.h>

int amp_symbols_init() __attribute__((weak));
int
amp_symbols_init()
{
	extern char *__progname;
	char progname[80];
	long storage;
	asymbol **symbol_table;
	long n_symbols;
	long f_symbols;
	long symbol_value;
	bfd *abfd;
	long i;

	bfd_init();		/* initialize magical internal data
				 * structures */
	progname[0] = '.';
	progname[1] = '/';
	progname[2] = '\0';

	strlcat(progname, __progname, 80);

	/*
	 * open objectect
	 */

	if ((abfd = bfd_openr(progname, NULL)) == NULL) {
		PUTS(*, "bfd: bfd_openr(\"%s\")=%p. file not found!\n", progname, abfd);
		return (-1);
	}
	PUTS(>, "bfd: bfd_openr(\"%s\")=%p\n", progname, abfd);

	/*
	 * check format
	 */

	if (bfd_check_format(abfd, bfd_object) == false) {
		PUTS(*, "bfd: bfd_check_format() == false (error)\n");
		return (-1);
	}
	/*
	 * calculate the storage space required
	 */

	storage = bfd_get_symtab_upper_bound(abfd);

	if (storage <= 0) {
		PUTS(*, "bfd: bfd_get_symtab_upper_bound(%p)=%d (error)\n", abfd, storage);
		return (-1);
	}
	PUTS(>, "bfd: bfd_get_symtab_upper_bound(%p)=%d bytes\n", abfd, storage);

	symbol_table = (asymbol **) exec(malloc, storage);

	/*
	 * retrive the number of symbols stored.
	 */

	f_symbols = n_symbols =
		bfd_canonicalize_symtab(abfd, symbol_table);

	if (n_symbols < 0) {
		PUTS(*, "bfd: bfd_canonicalize_symtab(%p,%p)=%d (error)\n", abfd, symbol_table, n_symbols);
		return (-1);
	}
	if (n_symbols == 0) {
		PUTS(>, "bfd: %s: executable, stripped\n", __progname, n_symbols);
		__options.stripped = 1;
		return (0);
	}
	PUTS(>, "bfd: %d symbols loaded\n", n_symbols);

	/*
	 * filter symbols
	 */

	for (i = 0; i < n_symbols; i++) {

		/*
		 * note: skip debugging symbol. ( some executable formats keep debugging symbols
		 * within the .text section. ( OpenBSD).
		 */

		if (symbol_table[i]->flags & BSF_DEBUGGING)
			continue;
		/*
		 * add the base section address, to the relative symbol value.
		 */

		symbol_value = symbol_table[i]->section != (asection *) NULL ?
			symbol_table[i]->value + symbol_table[i]->section->vma :
			symbol_table[i]->value;

		if (symbol_value) {
			__global.symb_low = MIN(__global.symb_low, symbol_value);
			__global.symb_high = MAX(__global.symb_high, symbol_value);
		}
		add_symbol(symbol_table[i]->name, symbol_value, symbol_table[i]->flags);
		f_symbols--;
	}

	PUTS(>, "bfd: %d symbols discarded\n", f_symbols);

	//dump_symbols();
	return (n_symbols);
}
