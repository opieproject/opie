TEMPLATE = lib
CONFIG -= moc
CONFIG += debug

HEADERS = cardmode.hh log.hh proto.hh sniff.hh sock.hh
SOURCES = cardmode.cc log.cc proto.cc sniff.cc sock.cc

#INCLUDEPATH += ../ ../library
#DEPENDPATH      += $(OPIEDIR)/include ../ ../library

LIBS =
TMAKE_CFLAGS += -D__UNIX__

DESTDIR = $(OPIEDIR)/noncore/net/wellenreiter/libwellenreiter/source
TARGET = wellenreiter
