#
#  almepro -- allocation memory profiler library
# 
#  Copyright (c) 2002 Bonelli Nicola <bonelli@blackhats.it>
# 
#  All rights reserved.
#

lib_LTLIBRARIES = libalmepro.la 

SRC    =  almepro.c amp_chunk.c amp_symbols.c amp_sensor.c amp_fatal.c amp_bfd.c printf.c 
EXTRASRCS = @LTLIBOBJS@

libalmepro_la_SOURCES = $(SRC) 
libalmepro_la_LIBADD = $(EXTRASRCS) 
