CONFIG		= qt warn_on release quick-app
HEADERS		= backuprestore.h
SOURCES		= main.cpp backuprestore.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES	= backuprestorebase.ui errordialog.ui
TARGET		= backup

TRANSLATIONS = ../../../i18n/de/backup.ts \
	 ../../../i18n/nl/backup.ts \
	../../../i18n/xx/backup.ts \
	../../../i18n/en/backup.ts \
	../../../i18n/es/backup.ts \
	../../../i18n/fr/backup.ts \
	../../../i18n/hu/backup.ts \
	../../../i18n/ja/backup.ts \
	../../../i18n/ko/backup.ts \
	../../../i18n/no/backup.ts \
	../../../i18n/pl/backup.ts \
	../../../i18n/pt/backup.ts \
	../../../i18n/pt_BR/backup.ts \
	../../../i18n/sl/backup.ts \
	../../../i18n/zh_CN/backup.ts \
	../../../i18n/zh_TW/backup.ts \
	../../../i18n/it/backup.ts \
	../../../i18n/da/backup.ts


include ( $(OPIEDIR)/include.pro )
