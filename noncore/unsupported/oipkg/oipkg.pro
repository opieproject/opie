DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
#CONFIG		= qt warn_on release
CONFIG		= qt warn_on debug
HEADERS		= mainwindow.h \
		pksettings.h \
		pmipkg.h \
		packagelistdoclnk.h \
		installdialog.h \
		utils.h \
		listviewitemoipkg.h \
		packagelistitem.h \
		packagelistremote.h \
		packagelist.h \
		packagelistlocal.h \
		packagelistview.h \
		package.h
SOURCES		= main.cpp \
		mainwindow.cpp \
		utils.cpp \
		packagelistdoclnk.cpp \
		packagelistview.cpp \
		installdialog.cpp \
		listviewitemoipkg.cpp \
		packagelistremote.cpp \
		packagelistlocal.cpp \
		pksettings.cpp \
		pmipkg.cpp \
		packagelistitem.cpp \
		packagelist.cpp \
		package.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/ioclude
LIBS            += -lqpe
LIBS            += -lopie
INTERFACES	= runwindow.ui \
		pksettingsbase.ui
TARGET		= oipkg

TRANSLATIONS = ../../../i18n/de/oipkg.ts \
	 ../../../i18n/en/oipkg.ts \
	 ../../../i18n/es/oipkg.ts \
	 ../../../i18n/fr/oipkg.ts \
	 ../../../i18n/hu/oipkg.ts \
	 ../../../i18n/ja/oipkg.ts \
	 ../../../i18n/ko/oipkg.ts \
	 ../../../i18n/no/oipkg.ts \
	 ../../../i18n/pl/oipkg.ts \
	 ../../../i18n/pt/oipkg.ts \
	 ../../../i18n/pt_BR/oipkg.ts \
	 ../../../i18n/sl/oipkg.ts \
	 ../../../i18n/zh_CN/oipkg.ts \
	 ../../../i18n/zh_TW/oipkg.ts
