TEMPLATE        = app

CONFIG          += qt warn_on release
DESTDIR         = ../bin

HEADERS		= d3des.h \
		  krfbbuffer.h \
		  krfbcanvas.h \
		  krfbconnection.h \
		  krfbdecoder.h \
		  krfblogin.h \
		  krfboptions.h \
		  krfbserverinfo.h \
		  kvnc.h \
		  kvncconnectdlg.h \
		  kvncoptionsdlg.h \
		  version.h \
		  vncauth.h

SOURCES		= d3des.c \
		  vncauth.c \
		  krfbbuffer.cpp \
		  krfbcanvas.cpp \
		  krfbconnection.cpp \
		  krfbdecoder.cpp \
		  krfblogin.cpp \
		  krfboptions.cpp \
		  kvnc.cpp \
		  kvncconnectdlg.cpp \
		  kvncoptionsdlg.cpp \
		  main.cpp
INTERFACES	= vncoptionsbase.ui
TARGET          = keypebble

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

TRANSLATIONS = ../i18n/pt_BR/keypebble.ts
TRANSLATIONS   += ../i18n/de/keypebble.ts
TRANSLATIONS   += ../i18n/en/keypebble.ts
TRANSLATIONS   += ../i18n/hu/keypebble.ts
TRANSLATIONS   += ../i18n/fr/keypebble.ts
TRANSLATIONS   += ../i18n/ja/keypebble.ts
TRANSLATIONS   += ../i18n/ko/keypebble.ts
TRANSLATIONS   += ../i18n/no/keypebble.ts
TRANSLATIONS   += ../i18n/zh_CN/keypebble.ts
TRANSLATIONS   += ../i18n/zh_TW/keypebble.ts
