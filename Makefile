CC=gcc
CFLAGS=-O3 -Wall `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`

.PHONY: all

all: rundlg

rundlg: rundlg.c
	$(CC) -Wall -o rundlg rundlg.c $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f rundlg *.o
