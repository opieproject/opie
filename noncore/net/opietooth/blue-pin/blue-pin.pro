TEMPLATE        = app
#CONFIG         = qt warn_on debug
CONFIG          = qt warn_on release
HEADERS         = pindlg.h 
SOURCES         = main.cc pindlg.cc
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH    += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe 
INTERFACES      = pindlgbase.ui
DESTDIR		= $(OPIEDIR)/bin
TARGET          = bluepin

TRANSLATIONS = ../../../../i18n/de/bluepin.ts \
	 ../../../../i18n/en/bluepin.ts \
	 ../../../../i18n/es/bluepin.ts \
	 ../../../../i18n/fr/bluepin.ts \
	 ../../../../i18n/hu/bluepin.ts \
	 ../../../../i18n/ja/bluepin.ts \
	 ../../../../i18n/ko/bluepin.ts \
	 ../../../../i18n/no/bluepin.ts \
	 ../../../../i18n/pl/bluepin.ts \
	 ../../../../i18n/pt/bluepin.ts \
	 ../../../../i18n/pt_BR/bluepin.ts \
	 ../../../../i18n/sl/bluepin.ts \
	 ../../../../i18n/zh_CN/bluepin.ts \
	 ../../../../i18n/zh_TW/bluepin.ts
