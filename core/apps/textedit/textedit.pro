TEMPLATE  = app
CONFIG    += qt warn_on release

DESTDIR   = $(OPIEDIR)/bin
HEADERS = textedit.h fileBrowser.h fontDialog.h fileSaver.h filePermissions.h 
SOURCES = main.cpp textedit.cpp fileBrowser.cpp fontDialog.cpp fileSaver.cpp filePermissions.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie

TARGET    = textedit

TRANSLATIONS += ../i18n/de/textedit.ts
TRANSLATIONS += ../i18n/pt_BR/textedit.ts
TRANSLATIONS   += ../i18n/en/textedit.ts
TRANSLATIONS   += ../i18n/hu/textedit.ts
TRANSLATIONS   += ../i18n/fr/textedit.ts
TRANSLATIONS   += ../i18n/ja/textedit.ts
TRANSLATIONS   += ../i18n/sl/textedit.ts
TRANSLATIONS   += ../i18n/pl/textedit.ts
TRANSLATIONS   += ../i18n/ko/textedit.ts
TRANSLATIONS   += ../i18n/no/textedit.ts
TRANSLATIONS   += ../i18n/zh_CN/textedit.ts
TRANSLATIONS   += ../i18n/zh_TW/textedit.ts
