TEMPLATE        = app
#CONFIG         = qt warn_on debug
CONFIG          = qt warn_on release
HEADERS         = pindlg.h 
SOURCES         = main.cc pindlg.cc
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe 
INTERFACES      = pindlgbase.ui
DESTDIR		= $(OPIEDIR)/bin
TARGET          = bluepin



TRANSLATIONS = ../../../../i18n/de/bluepin.ts
TRANSLATIONS += ../../../../i18n/en/bluepin.ts
TRANSLATIONS += ../../../../i18n/es/bluepin.ts
TRANSLATIONS += ../../../../i18n/fr/bluepin.ts
TRANSLATIONS += ../../../../i18n/hu/bluepin.ts
TRANSLATIONS += ../../../../i18n/ja/bluepin.ts
TRANSLATIONS += ../../../../i18n/ko/bluepin.ts
TRANSLATIONS += ../../../../i18n/no/bluepin.ts
TRANSLATIONS += ../../../../i18n/pl/bluepin.ts
TRANSLATIONS += ../../../../i18n/pt/bluepin.ts
TRANSLATIONS += ../../../../i18n/pt_BR/bluepin.ts
TRANSLATIONS += ../../../../i18n/sl/bluepin.ts
TRANSLATIONS += ../../../../i18n/zh_CN/bluepin.ts
TRANSLATIONS += ../../../../i18n/zh_TW/bluepin.ts

