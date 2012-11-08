PREFIX	= $(shell pwd)/../../

BINARY		= verify_capture
LIBS		= libpfcmu libviewplus

include $(PREFIX)/Makefile.cfg
include $(PREFIX)/bin/Makefile.bin

CFLAGS		+= `pkg-config --cflags opencv`
CXXFLAGS	+= `pkg-config --cflags opencv`
LDFLAGS		+= `pkg-config --libs opencv`

include $(DEPRULE)

