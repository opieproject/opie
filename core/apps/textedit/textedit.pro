TEMPLATE  = app
CONFIG    += qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS = textedit.h filePermissions.h 
SOURCES = main.cpp textedit.cpp filePermissions.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET    = textedit

TRANSLATIONS = ../../../i18n/de/textedit.ts \
	 ../../../i18n/da/textedit.ts \
	 ../../../i18n/xx/textedit.ts \
         ../../../i18n/en/textedit.ts \
         ../../../i18n/es/textedit.ts \
         ../../../i18n/fr/textedit.ts \
         ../../../i18n/hu/textedit.ts \
         ../../../i18n/ja/textedit.ts \
         ../../../i18n/ko/textedit.ts \
         ../../../i18n/no/textedit.ts \
         ../../../i18n/pl/textedit.ts \
         ../../../i18n/pt/textedit.ts \
         ../../../i18n/pt_BR/textedit.ts \
         ../../../i18n/sl/textedit.ts \
         ../../../i18n/zh_CN/textedit.ts \
         ../../../i18n/zh_TW/textedit.ts
