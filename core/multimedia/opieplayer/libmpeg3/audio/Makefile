include ../global_config
export CFLAGS
export CFLAGS_lessopt

OBJS = \
	ac3.o \
	bit_allocation.o \
	dct.o \
	exponents.o \
	header.o \
	layer2.o \
	layer3.o \
	mantissa.o \
	mpeg3audio.o \
	pcm.o \
	synthesizers.o \
	tables.o

all: $(OBJS)

dct.o: dct.c
	$(CC) -c `./c_flags dct.c` -o $@ $<

synthesizers.o: synthesizers.c
	$(CC) -c `./c_flags synthesizers.c` -o $@ $<


.c.o:
	$(CC) -c `./c_flags` -o $@ $<

.s.o:
	$(CC) -f elf $*.s

clean:
	rm -f *.o 
