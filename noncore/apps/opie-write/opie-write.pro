TEMPLATE	= app
CONFIG		+= qt warn_on release

DESTDIR		= $(OPIEDIR)/bin

HEADERS	=	qcleanuphandler.h \
		qcomplextext_p.h \
		qrichtext_p.h \
		qstylesheet.h \
		qtextedit.h \
		mainwindow.h

SOURCES	=	qcomplextext.cpp \
		qstylesheet.cpp \
		qrichtext_p.cpp \
		qrichtext.cpp \
		qtextedit.cpp \
		main.cpp \
		mainwindow.cpp

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe

TARGET		= opie-write

TRANSLATIONS = ../../../i18n/de/opie-write.ts \
	 ../../../i18n/nl/opie-write.ts \
         ../../../i18n/da/opie-write.ts \
         ../../../i18n/xx/opie-write.ts \
   ../../../i18n/en/opie-write.ts \
   ../../../i18n/es/opie-write.ts \
   ../../../i18n/fr/opie-write.ts \
   ../../../i18n/hu/opie-write.ts \
   ../../../i18n/ja/opie-write.ts \
   ../../../i18n/ko/opie-write.ts \
   ../../../i18n/no/opie-write.ts \
   ../../../i18n/pl/opie-write.ts \
   ../../../i18n/pt/opie-write.ts \
   ../../../i18n/pt_BR/opie-write.ts \
   ../../../i18n/sl/opie-write.ts \
   ../../../i18n/zh_CN/opie-write.ts \
   ../../../i18n/zh_TW/opie-write.ts


include ( $(OPIEDIR)/include.pro )
