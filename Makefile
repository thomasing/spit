all:
	gcc main.c -Wall -o spit
install:
	sudo cp spit /usr/local/bin/
