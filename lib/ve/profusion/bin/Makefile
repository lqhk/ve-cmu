DIRS	= $(patsubst %/Makefile,%,$(wildcard */Makefile))

all:
	@(set -e; for i in $(DIRS); do make -C $$i; done)

clean:
	find -type f -name '*.d' | xargs rm -f
	for i in $(DIRS); do make -C $$i clean; done

