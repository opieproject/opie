DESTDIR = $(OPIEDIR)/lib
TEMPLATE = lib
CONFIG = warn_on debug
VERSION = 0.2
HEADERS = source/cardmode.hh source/wl_log.hh source/wl_proto.hh source/sniff.hh source/wl_sock.hh source/wl_types.hh
SOURCES = source/cardmode.cc source/wl_log.cc source/wl_proto.cc source/sniff.cc source/wl_sock.cc

LIBS = -lpcap
TMAKE_CFLAGS += -D__UNIX__
DEFINES += DEBUG
TARGET = wellenreiter

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

