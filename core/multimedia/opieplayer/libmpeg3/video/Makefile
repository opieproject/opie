include ../global_config
export CFLAGS
export CFLAGS_lessopt

OBJS = \
	getpicture.o \
	headers.o \
	idct.o \
	macroblocks.o \
	mmxtest.o \
	motion.o \
	mpeg3video.o \
	output.o \
	reconstruct.o \
	seek.o \
	slice.o \
	vlc.o


all: $(OBJS) $(MMXOBJS2)

.c.o:
	$(CC) -c `./c_flags` -o $@ $<

.s.o:
	$(NASM) -f elf $*.s

.S.o:
	$(CC) -S `./c_flags` $*.S

clean:
	rm -f *.o 
