PREFIX	= $(shell pwd)/../../

BINARY		= server client
LIBS		= libpfcmu libviewplus

include $(PREFIX)/Makefile.cfg
include $(PREFIX)/bin/Makefile.bin

CFLAGS		+= `pkg-config --cflags opencv`
CXXFLAGS	+= `pkg-config --cflags opencv`
LDFLAGS		+= `pkg-config --libs opencv` -lboost_system

include $(DEPRULE)

