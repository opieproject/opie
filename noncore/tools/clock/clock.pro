TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = clock.h setAlarm.h
SOURCES   = clock.cpp setAlarm.cpp \
      main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES  = 
TARGET    = clock

TRANSLATIONS = ../i18n/de/clock.ts
TRANSLATIONS += ../i18n/pt_BR/clock.ts
