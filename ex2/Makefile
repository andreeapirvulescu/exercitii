CC=gcc
LIBQUEUE=-lrt
CCFLAGS=-Wall -g
SRV=server
SEL_SRV=server
CLT=client

build: $(SEL_SRV) $(CLT)

$(SEL_SRV):$(SEL_SRV).c
	$(CC) -o $(SEL_SRV) $(SEL_SRV).c $(LIBQUEUE)

$(CLT):	$(CLT).c
	$(CC) -o $(CLT) $(CLT).c $(LIBQUEUE)

clean:
	rm -f *.o *~
	rm -f $(SEL_SRV) $(CLT)