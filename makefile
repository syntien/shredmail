shredmail: shredmail.cc
	cc -o shredmail -I/usr/local/include -L/usr/local/lib -lstdc++ shredmail.cc

install: shredmail
	mv shredmail /usr/local/bin/
