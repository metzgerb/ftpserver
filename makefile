CC = gcc
CFLAGS = -g -Wall -std=c99

all: ftclient ftserve

ftserve: ftserve.c
	$(CC) $(CFLAGS) -o $@ $^
	
ftclient: ftclient.py
	chmod +x $^

clean: 
	chmod -x ./ftclient.py
	rm -rf ./ftserve