TEMPLATE = app
CONFIG += qt release

# Input
HEADERS += calc.h plugininterface.h instruction.h engine.h stdinstructions.h
SOURCES += calc.cpp main.cpp engine.cpp

INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include 
LIBS += -lqpe -Wl,-export-dynamic 



TRANSLATIONS = ../../../i18n/de/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/en/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/es/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/fr/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/hu/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/ja/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/ko/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/no/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/pl/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/pt/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/pt_BR/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/sl/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/zh_CN/calc.pro.in.ts
TRANSLATIONS += ../../../i18n/zh_TW/calc.pro.in.ts

