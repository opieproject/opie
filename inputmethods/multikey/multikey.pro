TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   keyboard.h \
            configdlg.h \
	    keyboardimpl.h 
SOURCES	=   keyboard.cpp \
            configdlg.cpp \
	    keyboardimpl.cpp
TARGET		= qmultikey
DESTDIR		= $(OPIEDIR)/plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe -L$(OPIEDIR)/plugins/inputmethods -lqpickboard
VERSION		= 1.0.0

TRANSLATIONS    = ../../i18n/pt_BR/libqmultikey.ts
    ../../i18n/de/libqmultikey.ts \
    ../../i18n/en/libqmultikey.ts \
    ../../i18n/hu/libqmultikey.ts \
    ../../i18n/sl/libqmultikey.ts \
    ../../i18n/ja/libqmultikey.ts \
    ../../i18n/ko/libqmultikey.ts \
    ../../i18n/pl/libqmultikey.ts \
    ../../i18n/no/libqmultikey.ts \
    ../../i18n/zh_CN/libqmultikey.ts \
    ../../i18n/zh_TW/libqmultikey.ts \
    ../../i18n/fr/libqmultikey.ts \
    ../../i18n/da/libqmultikey.ts
