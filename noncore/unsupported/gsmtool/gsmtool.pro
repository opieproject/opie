TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= gsmtool.h
SOURCES		= main.cpp gsmtool.cpp
# This doesn't actually work...
TMAKE_CXXFLAGS  += -fexceptions
INCLUDEPATH	+= $(OPIEDIR)/include
INCLUDEPATH	+= $(GSMLIBDIR)
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -L$(GSMLIBDIR)/gsmlib/.libs -lgsmme
INTERFACES	= gsmtoolbase.ui
TARGET		= gsmtool

TRANSLATIONS = ../../../i18n/de/gsmtool.ts \
	 ../../../i18n/en/gsmtool.ts \
	 ../../../i18n/es/gsmtool.ts \
	 ../../../i18n/fr/gsmtool.ts \
	 ../../../i18n/hu/gsmtool.ts \
	 ../../../i18n/ja/gsmtool.ts \
	 ../../../i18n/ko/gsmtool.ts \
	 ../../../i18n/no/gsmtool.ts \
	 ../../../i18n/pl/gsmtool.ts \
	 ../../../i18n/pt/gsmtool.ts \
	 ../../../i18n/pt_BR/gsmtool.ts \
	 ../../../i18n/sl/gsmtool.ts \
	 ../../../i18n/zh_CN/gsmtool.ts \
	 ../../../i18n/zh_TW/gsmtool.ts
