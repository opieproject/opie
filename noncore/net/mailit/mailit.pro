TEMPLATE  = app
CONFIG    = qt warn_on release
HEADERS   = emailclient.h \
      emailhandler.h \
      emaillistitem.h \
      mailitwindow.h \
      md5.h \
      popclient.h \
      readmail.h \
      smtpclient.h \
      writemail.h \
      textparser.h \
      viewatt.h \
      addatt.h \
      editaccount.h \
      maillist.h \
      addresslist.h
SOURCES   = emailclient.cpp \
      emailhandler.cpp \
      emaillistitem.cpp \
      mailitwindow.cpp \
      main.cpp \
      md5.c \
      popclient.cpp \
      readmail.cpp \
      smtpclient.cpp \
      writemail.cpp \
      textparser.cpp \
      viewatt.cpp \
      addatt.cpp \
      editaccount.cpp \
      maillist.cpp \
      addresslist.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include 
LIBS            += -lqpe -lopie
# -lssl
MOC_DIR=qpeobj
OBJECTS_DIR=qpeobj
DESTDIR=$(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/mailit.ts \
	 ../../../i18n/da/mailit.ts \
	 ../../../i18n/xx/mailit.ts \
	 ../../../i18n/en/mailit.ts \
	 ../../../i18n/es/mailit.ts \
	 ../../../i18n/fr/mailit.ts \
	 ../../../i18n/hu/mailit.ts \
	 ../../../i18n/ja/mailit.ts \
	 ../../../i18n/ko/mailit.ts \
	 ../../../i18n/no/mailit.ts \
	 ../../../i18n/pl/mailit.ts \
	 ../../../i18n/pt/mailit.ts \
	 ../../../i18n/pt_BR/mailit.ts \
	 ../../../i18n/sl/mailit.ts \
	 ../../../i18n/zh_CN/mailit.ts \
	 ../../../i18n/zh_TW/mailit.ts

include ( $(OPIEDIR)/include.pro )
