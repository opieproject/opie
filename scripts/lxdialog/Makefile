HOST_CFLAGS := -DLOCALE 
HOST_LDFLAGS   := -lncurses

ifeq (/usr/include/ncurses/ncurses.h, $(wildcard /usr/include/ncurses/ncurses.h))
        HOST_CFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses.h>"
else
ifeq (/usr/include/ncurses/curses.h, $(wildcard /usr/include/ncurses/curses.h))
        HOST_CFLAGS += -I/usr/include/ncurses -DCURSES_LOC="<ncurses/curses.h>"
else
ifeq (/usr/include/ncurses.h, $(wildcard /usr/include/ncurses.h))
        HOST_CFLAGS += -DCURSES_LOC="<ncurses.h>"
else
	HOST_CFLAGS += -DCURSES_LOC="<curses.h>"
endif
endif
endif

host-progs := lxdialog

lxdialog-objs := checklist.o menubox.o textbox.o yesno.o inputbox.o \
		 util.o lxdialog.o msgbox.o

EXTRA_TARGETS := lxdialog

first_rule: ncurses

include $(TOPDIR)/Rules.make

.PHONY: ncurses

ncurses:
	@echo "main() {}" > lxtemp.c
	@if $(HOSTCC) $(HOST_CFLAGS) lxtemp.c  $(HOST_LDFLAGS); then \
		rm -f lxtemp.c a.out; \
	else \
		rm -f lxtemp.c; \
		echo -e "\007" ;\
		echo ">> Unable to find the Ncurses libraries." ;\
		echo ">>" ;\
		echo ">> You must have Ncurses installed in order" ;\
		echo ">> to use 'make menuconfig'" ;\
		echo ;\
		exit 1 ;\
	fi
