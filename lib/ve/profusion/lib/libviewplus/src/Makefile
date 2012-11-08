BASENAME	= libviewplus
LIBOBJS		= \
		  PF_EZInterface.o \
		  PF_EZInterfaceLinux.o \

PREFIX	= $(shell pwd)/../../../

include $(PREFIX)/Makefile.cfg
include ../../$(BASENAME).cfg
include ../../Makefile.lib

CFLAGS          += -I../share -I../share/Linux
CXXFLAGS        += -I../share -I../share/Linux

include $(DEPRULE)

