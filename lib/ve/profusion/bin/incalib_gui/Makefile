PREFIX	= $(shell pwd)/../../

BINARY		= incalib
LIBS		= libpfcmu libviewplus

include $(PREFIX)/Makefile.cfg

include $(PREFIX)/bin/Makefile.bin

LDFLAGS		+= `pkg-config --libs opencv` -ltbb
CFLAGS		+= `pkg-config --cflags opencv`
CXXFLAGS	+= `pkg-config --cflags opencv`

include $(DEPRULE)
