CC=gcc
CFLAGS=
SRC=linkedlist.c setup.c pcap.c main.c

all: reimagined-garbanzo reimagined-garbanzo-d

reimagined-garbanzo-d:
	$(CC) $(CFLAGS) -o rgd -DDEBUG debug.c $(SRC)
reimagined-garbanzo:
	$(CC) $(CFLAGS) -o rg $(SRC)

clean:
	rm tdump
	rm tdumpd