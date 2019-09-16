#  This is the makefile for the microassembler. 5/7/91  BRL
#
CC=gcc
RM=rm -f
CCOPTS=-O3
BINDIR=/usr/local/bin
FTPDIR=/usr/local/ftp/pub/microprog/decmips
INSTALL=install -c -s
# 
INC1=masm.h global.h error.h evaluate.h version.h
INC2=masm.h global.h error.h symbol.h version.h
INC3=masm.h buffer.h global.h version.h
INC4=trans.h masm.h version.h
INC5=global.h error.h 
INC6=trans.h symbol.h buffer.h 
OBJS=getopt.o evaluate.o symbol.o buffer.o trans.o error.o masm.o
#
masm: masm1 masm2 masm2p masm3 masm4
#
masm1:   masm1.o $(OBJS)
	$(CC) $(CCOPTS) $(OBJS) masm1.o -o masm1
#
masm2:   masm2.o $(OBJS)
	$(CC) $(CCOPTS) $(OBJS) masm2.o -o masm2
#
masm2p:   masm2p.o $(OBJS)
	$(CC) $(CCOPTS) $(OBJS) masm2p.o -o masm2p
#
masm3:   masm3.o $(OBJS)
	$(CC) $(CCOPTS) $(OBJS) masm3.o -o masm3
#
masm4:   masm4.o $(OBJS)
	$(CC) $(CCOPTS) $(OBJS) masm4.o -o masm4
#
.c.o:
	$(CC) -c $(CCOPTS) $<
#
clean:
	$(RM) $(OBJS) masm1.o masm2.o masm2p.o masm3.o masm4.o core
#
realclean: clean
	$(RM) masm1 masm2 masm2p masm3 masm4
#
install: masm
	$(INSTALL) masm1 $(BINDIR)
	$(INSTALL) masm2 $(BINDIR)
	$(INSTALL) masm2p $(BINDIR)
	$(INSTALL) masm3 $(BINDIR)
	$(INSTALL) masm4 $(BINDIR)
	$(INSTALL) masm1 $(FTPDIR)
	$(INSTALL) masm2 $(FTPDIR)
	$(INSTALL) masm2p $(FTPDIR)
	$(INSTALL) masm3 $(FTPDIR)
	$(INSTALL) masm4 $(FTPDIR)
#
getopt.o: getopt.c
evaluate.o: evaluate.c $(INC1)
symbol.o:	symbol.c $(INC2)
buffer.o:	buffer.c $(INC3)
trans.o:	trans.c $(INC4)
error.o:	error.c $(INC5)
masm.o: 	masm.c $(INC1) $(INC6)
