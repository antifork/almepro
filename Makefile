#
#  almepro -- allocation memory profiler library
#
#  Copyright (c) 2002 Bonelli Nicola <bonelli@antifork.org>
#                     Banchi Leonardo <benkj@antifork.org>
#
#  All rights reserved.

SUBDIR		= src example script
MAKEFILES	= src/Makefile src/bsd_signal/Makefile script/Makefile \
	    		example/Makefile   

all: make_all

./config.status:
	./configure

$(MAKEFILES): ./config.status
	./config.status

make_all: $(MAKEFILES)
	@for i in $(SUBDIR); do (cd $$i; make); done

clean:
	@for i in $(SUBDIR); do (cd $$i; make clean); done

distclean: clean
	@for i in $(SUBDIR); do (cd $$i; make distclean); done
	-rm -fr autom4te* $(MAKEFILES) include/config.h *.{status,cache,log}
	-find . \( -name core -o -name \*.core -o -name \*~ -o -name \*.orig \)\
		                -exec rm -f '{}' \; -print

install: make_all
	@for i in $(SUBDIR); do (cd $$i; make install); done

uninstall:
	@for i in $(SUBDIR); do (cd $$i; make uninstall); done

