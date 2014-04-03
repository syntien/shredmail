shredmail: shredmail.cc
	cc -o shredmail -I/usr/local/include -L/usr/local/lib -lstdc++ shredmail.cc

install:
	test -f shredmail || $(MAKE) shredmail
	mv shredmail /usr/local/bin/
