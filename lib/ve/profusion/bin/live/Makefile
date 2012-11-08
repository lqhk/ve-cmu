PREFIX	= $(shell pwd)/../../

BINARY		= live
LIBS		= libviewplus

include $(PREFIX)/Makefile.cfg
include $(PREFIX)/bin/Makefile.bin

CFLAGS		+= `pkg-config --cflags gtkmm-2.4 libglademm-2.4`
CXXFLAGS	+= `pkg-config --cflags gtkmm-2.4 libglademm-2.4`
LDFLAGS		+= `pkg-config --libs gtkmm-2.4 libglademm-2.4`

include $(DEPRULE)

