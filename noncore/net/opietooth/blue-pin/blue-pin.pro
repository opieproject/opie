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
TARGET          = $(OPIEDIR)/bin/bluepin



TRANSLATIONS = ../../../../i18n/de/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/en/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/es/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/fr/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/hu/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/ja/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/ko/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/no/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/pl/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/pt/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/pt_BR/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/sl/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/zh_CN/$(OPIEDIR)/bin/bluepin.ts
TRANSLATIONS += ../../../../i18n/zh_TW/$(OPIEDIR)/bin/bluepin.ts

