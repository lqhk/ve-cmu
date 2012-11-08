PREFIX	= $(shell pwd)/../../

include $(PREFIX)/Makefile.cfg

all	: lib

lib	:
	@(cd src; make)

testprog	:
	@(cd test; make)

clean	:
	@(cd src; make clean)
#	@(cd test; make clean)
	@rm -vf *.o *.a *~ *.bak
	@find -name '*.d' | xargs rm -vf
