/* $Id$ */
/*
 * almepro -- Binary File Descriptor Library
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

#include <bfd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <amp.h>
#include <global.h>

static asymbol **symbol_table;
static bfd *abfd;

int amp_symbols_init() __attribute__((weak));
int
amp_symbols_init()
{
	extern char *__progname;
	char *progname;
	long storage;
	long n_symbols;
	long f_symbols;
	long symbol_value;
	long i;

	bfd_init();		/* initialize magical internal data
				 * structures */
	
	assert(progname = getenv("__AMP_TARGET"));

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
		__global.symb_low = 0;
		__global.symb_high = ULONG_MAX;
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

	if (__options.trace_all) {
		__global.symb_low = 0;
		__global.symb_high = ULONG_MAX;
	}
	
	//dump_symbols();
	return (n_symbols);
}

static char buff[128];

static char *get_file_name(char *);
static char *
get_file_name(s)
    register char *s;
{
	register char *ret, *pwd;
	char wd[128];

	/* s contains full-path source name */
	
	if (s == NULL || *s == '\0')
		return "??";
	
	switch (__options.src_path) {
	case OSRC_FULL :
		return s;
	case OSRC_BASE :
		ret = s;
		while (*s != '\0')
	  		if (*s++ == '/')
				ret = s;
		return ret;   
	case OSRC_REL :
		if (getcwd(wd, 128) == NULL)
			return s;
		ret = s;
		pwd = wd;
		buff[0] = '\0';

		while (*pwd == *s) {
			if (*s == '\0')
				return ret;
			pwd++;
			s++;
		}

		if (*s == '/')
			s++;
		else
			strlcat(buff, "../", 128);

		while (*pwd != '\0')
			if (*pwd++ == '/')
				strlcat(buff, "../", 128);

		if (buff[0] == '\0')
			strlcat(buff, "./", 128);

		strlcat(buff, s, 128);
		return buff;
	default :
	}
	
	return "";
}

char *amp_get_line(u_long) __attribute__((weak));
char *
amp_get_line(addr)
    u_long addr;
{
	bfd_vma vma, faddr;
	asection *sec;
	bfd_size_type size;
	const char *file, *func, *f;
	unsigned int line;
	char *ret;

	if (symbol_table == NULL || abfd == NULL || __options.src_path == OSRC_NONE)
		return "";

	faddr = (bfd_vma)addr;
	
	for (sec = abfd->sections; sec != NULL; sec = sec->next)
	{
		/* look for an address in a section */
		if ((bfd_get_section_flags(abfd, sec) & SEC_ALLOC) == 0)
			continue;
	
		vma = bfd_get_section_vma(abfd, sec);
		size = bfd_get_section_size_before_reloc(sec);

		if (faddr < vma || (size_t)(faddr - vma) >= size)
			continue;

		faddr -= vma;

		/* translate address into file_name:line_number */
		if (bfd_find_nearest_line(abfd, sec, symbol_table, faddr,
			    &file, &func, &line) && line > 0)
		{
			f = get_file_name((char *)file);

			assert(ret = (char *)exec(malloc, strlen(f) + 16));

			ksprintf(ret, "%s:%u ", f, line);
			
			return ret;
		}
	}

	return "";
}
