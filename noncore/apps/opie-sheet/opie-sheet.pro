TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= mainwindow.h sheet.h cellformat.h finddlg.h numberdlg.h sortdlg.h textdlg.h
SOURCES		= main.cpp mainwindow.cpp sheet.cpp cellformat.cpp finddlg.cpp numberdlg.cpp sortdlg.cpp textdlg.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= sheetqt

TRANSLATIONS = ../../../i18n/de/opie-sheet.ts \
	 ../../../i18n/en/opie-sheet.ts \
	 ../../../i18n/es/opie-sheet.ts \
	 ../../../i18n/fr/opie-sheet.ts \
	 ../../../i18n/hu/opie-sheet.ts \
	 ../../../i18n/ja/opie-sheet.ts \
	 ../../../i18n/ko/opie-sheet.ts \
	 ../../../i18n/no/opie-sheet.ts \
	 ../../../i18n/pl/opie-sheet.ts \
	 ../../../i18n/pt/opie-sheet.ts \
	 ../../../i18n/pt_BR/opie-sheet.ts \
	 ../../../i18n/sl/opie-sheet.ts \
	 ../../../i18n/zh_CN/opie-sheet.ts \
	 ../../../i18n/zh_TW/opie-sheet.ts \
   	 ../../../i18n/it/opie-sheet.ts
