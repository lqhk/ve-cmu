all	:
	make -C lib
	make -C bin

clean	:
	make -C bin clean
	make -C lib clean

tags	:
	find -iregex '.*[^.]+\.\(\(cc?\)\|\(h\)\|\(tcc\)\)$$' -print | etags -
