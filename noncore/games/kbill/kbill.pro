TEMPLATE 	= app
CONFIG		= qt warn_on release
HEADERS 	= kbill.h field.h Bucket.h Cable.h Computer.h Game.h Horde.h \
Library.h MCursor.h Monster.h Network.h Picture.h Spark.h Strings.h \
UI.h  objects.h inputbox.h
SOURCES		= field.cpp Bucket.cc Cable.cc Computer.cc Game.cc Horde.cc \
Library.cc MCursor.cc Monster.cc  Network.cc Picture.cc \ 
Spark.cc UI.cpp inputbox.cpp kbill.cpp
INTERFACES = helpdialog.ui
TARGET 		= kbill
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe
DESTDIR 	=  $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/kbill.ts \
	 ../../../i18n/en/kbill.ts \
	 ../../../i18n/es/kbill.ts \
	 ../../../i18n/fr/kbill.ts \
	 ../../../i18n/hu/kbill.ts \
	 ../../../i18n/ja/kbill.ts \
	 ../../../i18n/ko/kbill.ts \
	 ../../../i18n/no/kbill.ts \
	 ../../../i18n/pl/kbill.ts \
	 ../../../i18n/pt/kbill.ts \
	 ../../../i18n/pt_BR/kbill.ts \
	 ../../../i18n/sl/kbill.ts \
	 ../../../i18n/zh_CN/kbill.ts \
	 ../../../i18n/zh_TW/kbill.ts
