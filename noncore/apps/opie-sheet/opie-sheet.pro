TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= mainwindow.h sheet.h cellformat.h finddlg.h numberdlg.h sortdlg.h textdlg.h
SOURCES		= main.cpp mainwindow.cpp sheet.cpp cellformat.cpp finddlg.cpp numberdlg.cpp sortdlg.cpp textdlg.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= sheetqt

TRANSLATIONS = ../../../i18n/de/sheetqt.ts \
	 ../../../i18n/da/sheetqt.ts \
	 ../../../i18n/xx/sheetqt.ts \
	 ../../../i18n/en/sheetqt.ts \
	 ../../../i18n/es/sheetqt.ts \
	 ../../../i18n/fr/sheetqt.ts \
	 ../../../i18n/hu/sheetqt.ts \
	 ../../../i18n/ja/sheetqt.ts \
	 ../../../i18n/ko/sheetqt.ts \
	 ../../../i18n/no/sheetqt.ts \
	 ../../../i18n/pl/sheetqt.ts \
	 ../../../i18n/pt/sheetqt.ts \
	 ../../../i18n/pt_BR/sheetqt.ts \
	 ../../../i18n/sl/sheetqt.ts \
	 ../../../i18n/zh_CN/sheetqt.ts \
	 ../../../i18n/zh_TW/sheetqt.ts \
   	 ../../../i18n/it/sheetqt.ts
