#
# Makefile for bladeenc written by William Thompson <wakko@animx.eu.org>
# from compile script distributed with bladeenc.
# Modified a number of times by Tord Jansson <tord.jansson@swipnet.se>

# >>> Compiler options for GCC on i586 <<<
CFLAGS=-O2 -m486 -malign-jumps=2 -malign-loops=2 -funroll-all-loops
LIBS=-lm

# >>> Compiler options for GCC on SuperSparc Solaris <<<
#CFLAGS=-O2 -mv8 -msupersparc -funroll-all-loops
#LIBS=-lm

# >>> Compiler options for EGCS on UltraSparc Solaris <<<
#CFLAGS=-O2 -mv8 -mcpu=v9 -mcpu=ultrasparc -funroll-all-loops
#LIBS=-lm



OBJS=bladesys.o bladtab.o codec.o common.o encode.o formatbitstream2.o \
	huffman.o l3bitstream.o l3psy.o loop.o main.o mdct.o reservoir.o \
	samplein.o strupr.o subs.o tables.o

bladeenc: $(OBJS)
	gcc -o $@ $^ $(LIBS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

codec.o: codec.c
	gcc -O1 -c -o $@ $^
