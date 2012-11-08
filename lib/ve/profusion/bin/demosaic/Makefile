PREFIX	= $(shell pwd)/../../

BINARY		= demosaic
LIBS		= libpfcmu libviewplus

include $(PREFIX)/Makefile.cfg
include $(PREFIX)/bin/Makefile.bin

CFLAGS		+= `pkg-config --cflags opencv`
CXXFLAGS	+= `pkg-config --cflags opencv`
LDFLAGS		+= `pkg-config --libs opencv`

include $(DEPRULE)

