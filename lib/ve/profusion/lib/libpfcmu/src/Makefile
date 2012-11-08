BASENAME	= libpfcmu
LIBOBJS		= \
		capture.o \
		capture++.o \
		util.o \

PREFIX	= $(shell pwd)/../../../

include $(PREFIX)/Makefile.cfg
include ../../$(BASENAME).cfg
include ../../Makefile.lib

CFLAGS          += -I../share -I../share/Linux
CXXFLAGS        += -I../share -I../share/Linux

include $(DEPRULE)

