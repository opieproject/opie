TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   qimpenchar.h \
	    qimpenprofile.h \
	    qimpencombining.h \
	    qimpenhelp.h \
	    qimpeninput.h \
	    qimpenmatch.h \
	    qimpensetup.h \
	    qimpenstroke.h \
	    qimpenwidget.h \
	    qimpenwordpick.h \
	    handwritingimpl.h
SOURCES	=   qimpenchar.cpp \
	    qimpenprofile.cpp \
	    qimpencombining.cpp \
	    qimpenhelp.cpp \
	    qimpeninput.cpp \
	    qimpenmatch.cpp \
	    qimpensetup.cpp \
	    qimpenstroke.cpp \
	    qimpenwidget.cpp \
	    qimpenwordpick.cpp \
	    handwritingimpl.cpp
INTERFACES = qimpenprefbase.ui
TARGET		= qhandwriting
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe
VERSION		= 1.0.0

TRANSLATIONS = ../../i18n/de/libqhandwriting.ts \
	 ../../i18n/en/libqhandwriting.ts \
	 ../../i18n/es/libqhandwriting.ts \
	 ../../i18n/fr/libqhandwriting.ts \
	 ../../i18n/hu/libqhandwriting.ts \
	 ../../i18n/ja/libqhandwriting.ts \
	 ../../i18n/ko/libqhandwriting.ts \
	 ../../i18n/no/libqhandwriting.ts \
	 ../../i18n/pl/libqhandwriting.ts \
	 ../../i18n/pt/libqhandwriting.ts \
	 ../../i18n/pt_BR/libqhandwriting.ts \
	 ../../i18n/sl/libqhandwriting.ts \
	 ../../i18n/zh_CN/libqhandwriting.ts \
	 ../../i18n/zh_TW/libqhandwriting.ts
