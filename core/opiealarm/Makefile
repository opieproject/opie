
DESTDIR=$(OPIEDIR)/bin

CROSS:=arm-linux-
CC   :=$(CROSS)gcc
LD   :=$(CROSS)gcc
STRIP:=$(CROSS)strip

CFLAGS:=-O2
LDFLAGS:=

all: $(DESTDIR)/opiealarm

$(DESTDIR)/opiealarm: opiealarm.c
	$(CC) $(CFLAGS) opiealarm.c -o $(DESTDIR)/opiealarm $(LDFLAGS)
	$(STRIP) --strip-all $(DESTDIR)/opiealarm
	chmod u+s $(DESTDIR)/opiealarm
	chown root $(DESTDIR)/opiealarm 2>/dev/null || echo -e "\nopiealarm must be owned by root to work correctly.\n"

clean:
	-rm -f *~ core

