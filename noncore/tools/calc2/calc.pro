TEMPLATE = app
CONFIG += qt release

# Input
HEADERS += calc.h plugininterface.h instruction.h engine.h stdinstructions.h
SOURCES += calc.cpp main.cpp engine.cpp

INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include 
LIBS += -lqpe -Wl,-export-dynamic 

TRANSLATIONS = ../i18n/pt_BR/calc.ts
TRANSLATIONS   += ../i18n/de/calc.ts
TRANSLATIONS   += ../i18n/en/calc.ts
TRANSLATIONS   += ../i18n/sl/calc.ts
TRANSLATIONS   += ../i18n/hu/calc.ts
TRANSLATIONS   += ../i18n/fr/calc.ts
TRANSLATIONS   += ../i18n/ja/calc.ts
TRANSLATIONS   += ../i18n/pl/calc.ts
TRANSLATIONS   += ../i18n/ko/calc.ts
TRANSLATIONS   += ../i18n/no/calc.ts
TRANSLATIONS   += ../i18n/zh_CN/calc.ts
TRANSLATIONS   += ../i18n/zh_TW/calc.ts
