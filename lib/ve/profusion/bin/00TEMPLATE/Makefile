#
# Sample Makefile
#
# Suppose you have
#
#   bin1.cc
#   bin2.cc
#   common1.cc
#   common2.cc
#
# in this directory, and want to make two binaries bin1 and bin2 as
#
#   bin1 = bin1.o + common1.o + common2.o + libviewplus + libpfcmu + OpenCV
#   bin2 = bin2.o + common1.o + common2.o + libviewplus + libpfcmu + OpenCV
#
# To do this, you need to define
#
#   BINARY    = bin1 bin2
#   OBJS      = common1.o common2.o
#   LIBS      = libviewplus libpfcmu
#   CFLAGS   += `pkg-config --cflags opencv`
#   CXXFLAGS += `pkg-config --cflags opencv`
#   LDFLAGS  += `pkg-config --libs opencv`
#
#

PREFIX	= $(shell pwd)/../../

# We make two binaries.
BINARY		= bin1 bin2

# Object files linked with both bin1 and bin2
OBJS		= common1.o common2.o

# And they are linked with libpfcmu and libviewplus. The order here is very
# important. Keep it reflect the dependency (from left to right). See ld(1).
LIBS		= libpfcmu libviewplus

include $(PREFIX)/Makefile.cfg
include $(PREFIX)/bin/Makefile.bin

# Additional compile and link flags
CFLAGS		+= `pkg-config --cflags opencv`
CXXFLAGS	+= `pkg-config --cflags opencv`
LDFLAGS		+= `pkg-config --libs opencv`

include $(DEPRULE)

