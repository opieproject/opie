CONFIG		= qt warn_on release quick-app
HEADERS		= global.h \
		  mainwin.h \
		  datamgr.h \
		  settingsimpl.h \
		  ipkg.h \
		  packagewin.h \
		  package.h \
		  installdlgimpl.h \
		  instoptionsimpl.h \
		  destination.h \
		  utils.h \
		  server.h \
		  letterpushbutton.h \
		  inputdlg.h \
		  categoryfilterimpl.h
SOURCES		= mainwin.cpp \
		  datamgr.cpp \
		  mem.cpp \
		  settingsimpl.cpp \
		  ipkg.cpp \
		  main.cpp \
		  packagewin.cpp \
		  package.cpp \
		  installdlgimpl.cpp \
		  instoptionsimpl.cpp \
		  destination.cpp \
		  utils.cpp \
		  server.cpp \
		  letterpushbutton.cpp \
		  inputdlg.cpp \
		  version.cpp \
		  categoryfilterimpl.cpp
INTERFACES	= 
TARGET		= aqpkg
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie -lstdc++

TRANSLATIONS = ../../../i18n/de/aqpkg.ts \
	 ../../../i18n/nl/aqpkg.ts \
         ../../../i18n/xx/aqpkg.ts \
         ../../../i18n/en/aqpkg.ts \
         ../../../i18n/es/aqpkg.ts \
         ../../../i18n/fr/aqpkg.ts \
         ../../../i18n/hu/aqpkg.ts \
         ../../../i18n/ja/aqpkg.ts \
         ../../../i18n/ko/aqpkg.ts \
         ../../../i18n/no/aqpkg.ts \
         ../../../i18n/pl/aqpkg.ts \
         ../../../i18n/pt/aqpkg.ts \
         ../../../i18n/pt_BR/aqpkg.ts \
         ../../../i18n/sl/aqpkg.ts \
         ../../../i18n/zh_CN/aqpkg.ts \
         ../../../i18n/zh_TW/aqpkg.ts \
         ../../../i18n/it/aqpkg.ts \
         ../../../i18n/da/aqpkg.ts


include ( $(OPIEDIR)/include.pro )
