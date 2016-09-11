vntyper: main.cc process.cc
	g++ -std=c++11 $^ -o $@ `pkg-config --cflags --libs gtk+-3.0`

install:
	install vntyper /usr/bin
	install vntyper.cfg /etc
