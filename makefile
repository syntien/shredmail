shredmail: shredmail.cc
	cc -std=c++20 -stdlib=libc++ -o shredmail -I/usr/local/include -L/usr/local/lib -lstdc++ shredmail.cc

install: shredmail
	mv shredmail /usr/local/bin/

clean:
	@rm -f shredmail
