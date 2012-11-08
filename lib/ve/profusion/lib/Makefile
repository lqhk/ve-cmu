all	: lib

LIBS	= \
	libviewplus \
	libpfcmu \

lib	:
	@(set -e; for i in $(LIBS); do make -C $$i; done)

clean	:
	@for i in $(LIBS); do (cd $$i; make clean); done
	@find -type f -name '*.d' | xargs rm -vf

