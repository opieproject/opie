TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = opieftp.h inputDialog.h ftplib.h
SOURCES   = opieftp.cpp inputDialog.cpp ftplib.c main.cpp
TARGET    = opieftp
REQUIRES=medium-config
DESTDIR   = $(OPIEDIR)/bin
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS += ../../i18n/pt_BR/opieftp.ts
