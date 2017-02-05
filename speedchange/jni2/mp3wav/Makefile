SRCDIR=mpglib
CFLAGS=-I<Includepath>
CC=lcc.exe
LINKER=lcclnk.exe
OBJS=\
	common.obj \
	dct64_i386.obj \
	decode_i386.obj \
	interface.obj \
	layer2.obj \
	layer3.obj \
	tabinit.obj

LIBS=

mpglib.dll:	$(OBJS) Makefile
	$(LINKER)  -s -dll -o $(SRCDIR)\mpglib.dll $(OBJS) $(LIBS)

# Build COMMON.C
COMMON_C=\
	$(SRCDIR)\mpg123.h\

common.obj: $(COMMON_C) $(SRCDIR)\common.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\common.c

# Build DCT64_I386.C
DCT64_I386_C=\
	$(SRCDIR)\mpg123.h\

dct64_i386.obj: $(DCT64_I386_C) $(SRCDIR)\dct64_i386.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\dct64_i386.c

# Build DECODE_I386.C
DECODE_I386_C=\
	$(SRCDIR)\mpg123.h\
	$(SRCDIR)\mpglib.h\

decode_i386.obj: $(DECODE_I386_C) $(SRCDIR)\decode_i386.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\decode_i386.c

# Build INTERFACE.C
INTERFACE_C=\
	$(SRCDIR)\mpg123.h\
	$(SRCDIR)\mpglib.h\

interface.obj: $(INTERFACE_C) $(SRCDIR)\interface.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\interface.c

# Build LAYER2.C
LAYER2_C=\
	$(SRCDIR)\mpg123.h\
	$(SRCDIR)\l2tables.h\

layer2.obj: $(LAYER2_C) $(SRCDIR)\layer2.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\layer2.c

# Build LAYER3.C
LAYER3_C=\
	$(SRCDIR)\mpg123.h\
	$(SRCDIR)\mpglib.h\
	$(SRCDIR)\huffman.h\

layer3.obj: $(LAYER3_C) $(SRCDIR)\layer3.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\layer3.c

# Build TABINIT.C
TABINIT_C=\
	$(SRCDIR)\mpg123.h\

tabinit.obj: $(TABINIT_C) $(SRCDIR)\tabinit.c
	$(CC) -c $(CFLAGS) $(SRCDIR)\tabinit.c

link:
	$(LINKER)  -s -dll -o $(SRCDIR)\mpglib.dll $(OBJS) $(LIBS)

