/*
 * sensor.h
 *
 * Copyright (c) 2002 Nicola Bonelli <bonelli@blackhats.it>
 *
 * All rights reserved.
 *
 * Linux   2.4.x
 * OpenBSD 2.8|2.9|3.0|3.1|3.2
 * FreeBSD 4.4|4.5|4.6|4.7|5.0
 * NetBSD  1.5.2|1.6 (no siginfo support)
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


#define CVS		"$Id$"

#define CAT(a,b)       a ## b
#define INFO_PID()     printf("  pid= %d\n",		getpid())
#define INFO_HEX(x)    printf("  %s= %#x\n",		#x,(long)x)
#define INFO_INT(x)    printf("  si_%s= %d\n",		#x,(int)sip->si_##x)
#define INFO_ADR(x)    printf("  si_%s= %p\n",		#x,(void *)sip->si_##x)
#define INFO_CODE(x,c) printf("  si_code(%d)= %s\n",	c,x[c])

/* arch dependences */

#if ! #cpu (i386)
#error "The current sensor.h supports i386 hosts only"
#endif

#if   defined(__linux__)

#define SI_CODE_POSIX
#define HAVE_SIGINFO

#define ILL_CODE  	 ill_code
#define FPE_CODE  	 fpe_code
#define SEGV_CODE 	segv_code
#define BUS_CODE  	 bus_code
#define TRAP_CODE	trap_code
#define CLD_CODE 	 cld_code
#define POLL_CODE	poll_code

#define DUMP_REG(x)     printf("  %s\t%#x\t%d\n",(#x)+4,((ucontext_t *)scp)->uc_mcontext.gregs[x],\
                        ((ucontext_t *)scp)->uc_mcontext.gregs[x])

#define REG_gs         REG_GS
#define REG_fs         REG_FS
#define REG_es         REG_ES
#define REG_ds         REG_DS
#define REG_edi        REG_EDI
#define REG_esi        REG_ESI
#define REG_ebp        REG_EBP
#define REG_esp        REG_ESP
#define REG_ebx        REG_EBX
#define REG_edx        REG_EDX
#define REG_ecx        REG_ECX
#define REG_eax        REG_EAX
#define REG_eip        REG_EIP
#define REG_cs         REG_CS
#define REG_eflags     REG_EFL
#define REG_ss 	REG_SS


#elif defined(__OpenBSD__)

#define SI_CODE_POSIX
#define HAVE_SIGINFO

#define ILL_CODE         ill_code
#define FPE_CODE         fpe_code
#define SEGV_CODE       segv_code
#define BUS_CODE         bus_code
#define TRAP_CODE       trap_code
#define CLD_CODE         cld_code
#define POLL_CODE       poll_code

#define DUMP_REG(x)  	printf("  %s\t%#x\t%d\n", (#x)+4, \
			((struct sigcontext *)scp)->CAT(sc_,x), \
			((struct sigcontext *)scp)->CAT(sc_,x))

#define REG_gs        	gs
#define REG_fs        	fs
#define REG_es         es
#define REG_ds         ds
#define REG_edi        edi
#define REG_esi        esi
#define REG_ebp        ebp
#define REG_esp        esp
#define REG_ebx        ebx
#define REG_edx        edx
#define REG_ecx        ecx
#define REG_eax        eax
#define REG_eip        eip
#define REG_cs         cs
#define REG_eflags     eflags
#define REG_ss         ss

#elif defined(__FreeBSD__)

#define SI_CODE_TRAP
#define HAVE_SIGINFO

#define ILL_CODE        global_code
#define FPE_CODE           fpe_code
#define SEGV_CODE       global_code
#define BUS_CODE        global_code
#define TRAP_CODE       global_code
#define CLD_CODE        global_code
#define POLL_CODE       global_code

#define DUMP_REG(x)     printf("  %s\t%#x\t%d\n", (#x)+4, \
                        ((struct sigcontext *)scp)->CAT(sc_,x), \
                        ((struct sigcontext *)scp)->CAT(sc_,x))

#define REG_gs         gs
#define REG_fs         fs
#define REG_es         es
#define REG_ds         ds
#define REG_edi        edi
#define REG_esi        esi
#define REG_ebp        ebp
#define REG_esp        esp
#define REG_ebx        ebx
#define REG_edx        edx
#define REG_ecx        ecx
#define REG_eax        eax
#define REG_eip        eip
#define REG_cs         cs
#define REG_eflags     eflags
#define REG_ss         ss

#elif defined(__NetBSD__)

/* Struct siginfo not available yet!
   Future versions of NetBSD will replace the sigcontext interface with the siginfo interface.
*/

#define DUMP_REG(x)     printf("  %s\t%#x\t%d\n", (#x)+4, \
                        ((struct sigcontext *)scp)->CAT(sc_,x), \
                        ((struct sigcontext *)scp)->CAT(sc_,x))

#define REG_gs         gs
#define REG_fs         fs
#define REG_es         es
#define REG_ds         ds
#define REG_edi        edi
#define REG_esi        esi
#define REG_ebp        ebp
#define REG_esp        esp
#define REG_ebx        ebx
#define REG_edx        edx
#define REG_ecx        ecx
#define REG_eax        eax
#define REG_eip        eip
#define REG_cs         cs
#define REG_eflags     eflags
#define REG_ss         ss

#elif
#error "The current sensor.h doesn't support your unix flavor"
#endif

#define SIG(x)       [  SIG##x]  "SIG"  #x
#define ILL_(x,y)    [ ILL_##x]  "ILL_" #x "(" y ")"
#define FPE_(x,y)    [ FPE_##x]  "FPE_" #x "(" y ")"
#define BUS_(x,y)    [ BUS_##x]  "BUS_" #x "(" y ")"
#define CLD_(x,y)    [ CLD_##x]  "CLD_" #x "(" y ")"
#define SEGV_(x,y)   [SEGV_##x] "SEGV_" #x "(" y ")"
#define TRAP_(x,y)   [TRAP_##x] "TRAP_" #x "(" y ")"
#define POLL_(x,y)   [POLL_##x] "POLL_" #x "(" y ")"
#define T_(x,y)	     [T_##x]       "T_" #x "(" y ")"
