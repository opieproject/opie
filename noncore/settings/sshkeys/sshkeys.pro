TEMPLATE = app
CONFIG += qt warn_on release
HEADERS = sshkeys.h
SOURCES = main.cpp sshkeys.cpp
TARGET = sshkeys
INTERFACES = sshkeysbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include
LIBS += -lqpe -lopie
DESTDIR = $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/sshkeys.ts \
	../../../i18n/xx/sshkeys.ts \
	../../../i18n/en/sshkeys.ts \
	../../../i18n/es/sshkeys.ts \
	../../../i18n/fr/sshkeys.ts \
	../../../i18n/hu/sshkeys.ts \
	../../../i18n/ja/sshkeys.ts \
	../../../i18n/ko/sshkeys.ts \
	../../../i18n/no/sshkeys.ts \
	../../../i18n/pl/sshkeys.ts \
	../../../i18n/pt/sshkeys.ts \
	../../../i18n/pt_BR/sshkeys.ts \
	../../../i18n/sl/sshkeys.ts \
	../../../i18n/zh_CN/sshkeys.ts \
	../../../i18n/zh_TW/sshkeys.ts \
	../../../i18n/it/sshkeys.ts \
	../../../i18n/da/sshkeys.ts

