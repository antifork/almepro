/* $Id$ */
/*
 * almepro -- sensor.h http://bonelli.oltrelinux.com/codes/sensor.h
 *
 * Copyright (c) 2002 Bonelli Nicola <bonelli@antifork.org>
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

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#ifndef __linux__
#include <string.h>
#endif
#include <unistd.h>
#include <sensor.h>		/* sensor.h */
#define __USE_GNU
#include <dlfcn.h>
#ifdef linux			/* FIXME */
#include <ucontext.h>
#endif
#include <amp.h>
#include <global.h>

/* signal table */

char *sig_table[33] = {
	SIG(HUP), SIG(INT), SIG(QUIT), SIG(ILL), SIG(ABRT), SIG(FPE),
	SIG(KILL), SIG(SEGV), SIG(PIPE), SIG(ALRM), SIG(TERM), SIG(USR1),
	SIG(USR2), SIG(CHLD), SIG(CONT), SIG(STOP), SIG(TSTP), SIG(TTIN),
	SIG(TTOU), SIG(BUS),
#if defined (SIGPOLL)
	SIG(POLL),
#endif
	SIG(PROF), SIG(SYS), SIG(TRAP),
	SIG(URG), SIG(VTALRM), SIG(XCPU), SIG(XFSZ),

#if !defined(__NetBSD__)
	SIG(IOT),		/* compatibility */
#endif

#if defined (SIGSTKFLT)
	SIG(STKFLT),
#endif
	SIG(IO),
#if defined (SIGPWR)
	SIG(PWR),
#endif
	SIG(WINCH),
};


/* si_code */

#if defined (SI_CODE_POSIX)

char *ill_code[12] = {
	ILL_(ILLOPC, "illegal opcode"),
	ILL_(ILLOPN, "illegal operand"),
	ILL_(ILLADR, "illegal addressing mode"),
	ILL_(ILLTRP, "illegal trap"),
	ILL_(PRVOPC, "privileged opcode"),
	ILL_(PRVREG, "privileged register"),
	ILL_(COPROC, "co-processor"),
	ILL_(BADSTK, "bad stack"),
};

char *fpe_code[12] = {
	FPE_(INTDIV, "integer divide by zero"),
	FPE_(INTOVF, "integer overflow"),
	FPE_(FLTDIV, "floating point divide by zero"),
	FPE_(FLTOVF, "floating point overflow"),
	FPE_(FLTUND, "floating point underflow"),
	FPE_(FLTRES, "floating point inexact result"),
	FPE_(FLTINV, "invalid floating point operation"),
	FPE_(FLTSUB, "subscript out of range"),
};

char *segv_code[4] = {
	SEGV_(MAPERR, "address not mapped to object"),
	SEGV_(ACCERR, "invalid permissions"),
};

char *bus_code[4] = {
	BUS_(ADRALN, "invalid address alignment"),
	BUS_(ADRERR, "non-existent physical address"),
	BUS_(OBJERR, "object specific hardware error"),
};

char *trap_code[4] = {
	TRAP_(BRKPT, "breakpoint trap"),
	TRAP_(TRACE, "trace trap"),
};

char *cld_code[8] = {
	CLD_(EXITED, "child has exited"),
	CLD_(KILLED, "child was killed"),
	CLD_(DUMPED, "child has coredumped"),
	CLD_(TRAPPED, "traced child has stopped"),
	CLD_(STOPPED, "child has stopped on signal"),
	CLD_(CONTINUED, "stopped child has continued"),
};

#if defined (__linux__)
char *poll_code[8] = {
	POLL_(IN, "input available"),
	POLL_(OUT, "output possible"),
	POLL_(MSG, "message available"),
	POLL_(ERR, "I/O error"),
	POLL_(PRI, "high priority input available"),
	POLL_(HUP, "device disconnected"),
};
#endif
#endif

#if defined(SI_CODE_TRAP)

/* Freebsd flavor */

char *global_code[32] = {
	T_(PRIVINFLT, "privileged instruction"),
	T_(BPTFLT, "breakpoint instruction"),
	T_(ARITHTRAP, "arithmetic trap"),
#if defined(T_ASTFLT)
	T_(ASTFLT, "system forced exception"),
#endif
	T_(PROTFLT, "protection fault"),
	T_(TRCTRAP, "debug exception (sic)"),
	T_(PAGEFLT, "page fault"),
	T_(ALIGNFLT, "alignment fault"),

	T_(DIVIDE, "integer divide fault"),
	T_(NMI, "non-maskable trap"),
	T_(OFLOW, "overflow trap"),
	T_(BOUND, "bound instruction fault"),
	T_(DNA, "device not available fault"),
	T_(DOUBLEFLT, "double fault"),
	T_(FPOPFLT, "fp coprocessor operand fetch fault"),
	T_(TSSFLT, "invalid tss fault"),
	T_(SEGNPFLT, "segment not present fault"),
	T_(STKFLT, "stack fault"),
	T_(MCHK, "machine check trap"),
	T_(XMMFLT, "SIMD floating-point exception"),
	T_(RESERVED, "reserved (unknown)"),

	ILL_(PRIVIN_FAULT, "privileged instruction"),
	ILL_(ALIGN_FAULT, "alignment fault"),
	ILL_(FPOP_FAULT, "coprocessor operand fault"),

	BUS_(PAGE_FAULT, "page fault protection base"),
	BUS_(SEGNP_FAULT, "segment not present"),
	BUS_(STK_FAULT, "stack segment"),
	BUS_(SEGM_FAULT, "segment protection base"),

};

char *fpe_code[12] = {
	FPE_(INTOVF, "integer overflow"),
	FPE_(INTDIV, "integer divide by zero"),
	FPE_(FLTDIV, "floating point divide by zero"),
	FPE_(FLTOVF, "floating point overflow"),
	FPE_(FLTUND, "floating point underflow"),
	FPE_(FLTRES, "floating point inexact result"),
	FPE_(FLTINV, "invalid floating point operation"),
	FPE_(FLTSUB, "subscript out of range"),
};
#endif

#ifdef HAVE_SIGINFO
void amp_handler(int, siginfo_t *, void *) __attribute__((weak));
void
amp_handler(int sig, siginfo_t * sip, void *scp)
#else
void amp_handler(int, int, struct sigcontext *) __attribute__((weak));
void
amp_handler(int sig, int code, struct sigcontext * scp)
#endif
{
	int c, wstat;
	sigset_t set, oset;

	PUTS(>, "catched signal %s!\n", sig_table[sig]);

	INFO_PID();

#ifdef HAVE_SIGINFO

	INFO_INT(signo);
	INFO_INT(errno);

	switch (sig) {
	case SIGSEGV:
	case SIGBUS:
	case SIGILL:
	case SIGFPE:
		if (sip->si_addr)
			INFO_ADR(addr);
		break;

	case SIGCHLD:
		INFO_INT(pid);
		INFO_INT(uid);
		INFO_INT(status);

#if !defined(__FreeBSD__)
		INFO_INT(utime);
		INFO_INT(stime);
#endif

#if defined (SIGPOLL)
	case SIGPOLL:
#endif

#if defined (SIGXFSZ)
	case SIGXFSZ:
#endif

#if defined (SIGPOLL) || (SIGXFSZ)

#if defined (__linux__)
		INFO_INT(fd);
		INFO_INT(band);
		break;
#endif
#endif
		/* code */
		c = sip->si_code;

		switch (sig) {
		case SIGILL:
			INFO_CODE(ILL_CODE, c);
			break;
		case SIGFPE:
			INFO_CODE(FPE_CODE, c);
			break;
		case SIGSEGV:
			INFO_CODE(SEGV_CODE, c);
			break;
		case SIGBUS:
			INFO_CODE(BUS_CODE, c);
			break;
		case SIGTRAP:
			INFO_CODE(TRAP_CODE, c);
			break;
		case SIGCHLD:
			INFO_CODE(CLD_CODE, c);
			break;
#if defined (SIGPOLL)
		case SIGPOLL:
			INFO_CODE(POLL_CODE, c);
			break;
#endif
		}
	}
#else
	INFO_HEX(code);
#endif				/* HAVE_SIGINFO */

	PUTS(>, "context:");
	DUMP_REG(REG_eax);
	DUMP_REG(REG_ecx);
	DUMP_REG(REG_edx);
	DUMP_REG(REG_ebx);
	DUMP_REG(REG_esp);
	DUMP_REG(REG_ebp);
	DUMP_REG(REG_esi);
	DUMP_REG(REG_edi);
	DUMP_REG(REG_eip);
	DUMP_REG(REG_eflags);
	DUMP_REG(REG_cs);
	DUMP_REG(REG_ss);
	DUMP_REG(REG_ds);
	DUMP_REG(REG_es);
	DUMP_REG(REG_fs);
	DUMP_REG(REG_gs);
	puts("");

	switch ((int) __orig[sig].sa_handler) {
	case ((int) SIG_IGN):
		PUTS(>, "signal ignored.\n");
		break;
	case ((int) SIG_DFL):
		PUTS(>, "signal with default action.\n");

		/* Restore the default action, rasing a nested signal */
#ifdef RTLD_NEXT
		exec(sigaction, sig, &__orig[sig], NULL);
#else
		__amp_sigaction(sig, &__orig[sig], NULL);
#endif
		sigemptyset(&set);
		sigaddset(&set, sig);
		sigprocmask(SIG_UNBLOCK, &set, &oset);
		raise(sig);

		wait(&wstat);	/* wait the __orig hander's exit */

#ifdef RTLD_NEXT
		exec(sigaction, sig, &__hdr, NULL);
#else
		__amp_sigaction(sig, &__hdr, NULL);
#endif
		sigprocmask(SIG_SETMASK, &oset, NULL);
		break;

	default:
		PUTS(>, "signal handler at 0x%x\n", (int) __orig[sig].sa_handler);
		__orig[sig].sa_handler(sig);
		break;
	}
}


void dump_signal(int, void *, u_long) __attribute__((weak));
void
dump_signal(signum, handler, c_addr)
	int signum;
	void *handler;
	u_long c_addr;
{
	char *hdl;

	switch ((int) handler) {
	case ((int) SIG_IGN):
		PUTS($, "%p <%s>: call <signal(%s,SIG_IGN)>\n", c_addr, get_symb_descr(c_addr), sig_table[signum]);
		break;
	case ((int) SIG_DFL):
		PUTS($, "%p <%s>: call <signal(%s,SIG_DFL)>\n", c_addr, get_symb_descr(c_addr), sig_table[signum]);
		break;
	default:
		hdl = exec(strdup, (char *) get_symb_descr(handler));
		PUTS($, "%p <%s>: call <signal(%s,%s)>\n", c_addr, get_symb_descr(c_addr), sig_table[signum], hdl);
		exec(free, hdl);
	}

}


#ifdef HAVE_SIGINFO
void amp_ctrl_c(int, siginfo_t *, void *) __attribute__((weak));
void
amp_ctrl_c(int sig, siginfo_t * sip, void *scp)
#else
void amp_ctrl_c(int, int, struct sigcontext *) __attribute__((weak));
void
amp_ctrl_c(int sig, int code, struct sigcontext * scp)
#endif
{
	PUTS(>, "catched CTRL+C (signal %s)!\n", sig_table[sig]);
	exit(1);
}

int amp_sensor(int);
int
amp_sensor(int opt)
{
	int i;

#ifdef HAVE_SIGINFO
	__hdr.sa_flags = SA_SIGINFO;
	__hdr.sa_sigaction = amp_handler;
	__hdr_exit.sa_flags = SA_SIGINFO;
	__hdr_exit.sa_sigaction = amp_ctrl_c;
#else
	__hdr.sa_handler = (void (*) (int)) amp_handler;
	__hdr_exit.sa_handler = amp_ctrl_c;

#endif
#ifdef  SA_RESTART
	__hdr.sa_flags |= SA_RESTART;	/* SVR4, 44BSD */
#endif
	for (i = 0; i < 32; i++)
		switch (i) {
		case SIGINT: // happy ctrl_c >>> benkj
#ifdef RTLD_NEXT
				exec(sigaction, SIGINT, &__hdr_exit, NULL);
#else
				__amp_sigaction(SIGINT, &__hdr_exit, NULL);
#endif
			break;

		default: // masking signals with __hdr sigaction
#ifdef RTLD_NEXT
				exec(sigaction, i, &__hdr, &__orig[i]);
#else
				__amp_sigaction(i, &__hdr, &__orig[i]);
#endif
		}
	return;
}
