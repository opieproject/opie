TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= backuprestore.h
SOURCES		= main.cpp backuprestore.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= backuprestorebase.ui
TARGET		= backup
DESTDIR		= $(OPIEDIR)/bin	

TRANSLATIONS = ../../../i18n/de/backup.ts \
·    ../../../i18n/en/backup.ts \
·    ../../../i18n/es/backup.ts \
·    ../../../i18n/fr/backup.ts \
·    ../../../i18n/hu/backup.ts \
·    ../../../i18n/ja/backup.ts \
·    ../../../i18n/ko/backup.ts \
·    ../../../i18n/no/backup.ts \
·    ../../../i18n/pl/backup.ts \
·    ../../../i18n/pt/backup.ts \
·    ../../../i18n/pt_BR/backup.ts \
·    ../../../i18n/sl/backup.ts \
·    ../../../i18n/zh_CN/backup.ts \
·    ../../../i18n/zh_TW/backup.ts \
   · ../../../i18n/it/backup.ts \
      · ../../../i18n/da/backup.ts
	  
