TEMPLATE = lib
CONFIG -= moc
CONFIG += debug
VERSION = 0.1
HEADERS = source/cardmode.hh source/log.hh source/proto.hh source/sniff.hh source/sock.hh
SOURCES = source/cardmode.cc source/log.cc source/proto.cc source/sniff.cc source/sock.cc

#INCLUDEPATH += ../ ../library
#DEPENDPATH      += $(OPIEDIR)/include ../ ../library

LIBS =
TMAKE_CFLAGS += -D__UNIX__

DESTDIR = $(QTDIR)/lib
TARGET = wellenreiter
