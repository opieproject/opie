TEMPLATE	= lib

CONFIG		+= qt staticlib warn_on 

DESTDIR		= $$(OPIEDIR)/lib

HEADERS		+= \
	buf.h \
	checksum.h \
	command.h \
	emit.h \
	job.h \
	netint.h \
	protocol.h \
	prototab.h \
	rsync.h \
	search.h \
	stream.h \
	sumset.h \
	trace.h \
	types.h \
	util.h \
	whole.h \
	config_rsync.h \
	qrsync.h

SOURCES		+= \
	base64.c \
	buf.c \
	checksum.c \
	command.c \
	delta.c \
	emit.c \
	hex.c \
	job.c \
	mdfour.c \
	mksum.c \
	msg.c \
	netint.c \
	patch.c \
	prototab.c \
	readsums.c \
	scoop.c \
	search.c \
	stats.c \
	stream.c \
	sumset.c \
	trace.c \
	tube.c \
	util.c \
	version.c \
	whole.c \
	qrsync.cpp

INCLUDEPATH += $(OPIEDIR)/include .

LIBS        += -lopiecore2
TARGET       = qrsync

include ( $(OPIEDIR)/include.pro )
