multiprocess:TEMPLATE	= app
multiprocess:DESTDIR	= $(QPEDIR)/bin
singleprocess:TEMPLATE	= lib
singleprocess:DESTDIR   = $(QPEDIR)/lib
quicklaunch:TEMPLATE	= lib
quicklaunch:DESTDIR	= $(QPEDIR)/plugins/application

CONFIG	+= qtopia warn_on release
HEADERS		= clock.h \
		analogclock.h
SOURCES		= clock.cpp \
		analogclock.cpp

multiprocess:SOURCES+=main.cpp

INTERFACES	= clockbase.ui \
		alarmdlgbase.ui
TARGET		= clock

TRANSLATIONS = clock-en_GB.ts clock-de.ts clock-ja.ts clock-no.ts
