TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = opieftp.h inputDialog.h ftplib.h
SOURCES   = opieftp.cpp inputDialog.cpp main.cpp
TARGET    = opieftp
DESTDIR   = $(OPIEDIR)/bin
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lftplib

TRANSLATIONS = ../../../i18n/de/opieftp.ts \
	 ../../../i18n/en/opieftp.ts \
	 ../../../i18n/es/opieftp.ts \
	 ../../../i18n/fr/opieftp.ts \
	 ../../../i18n/hu/opieftp.ts \
	 ../../../i18n/ja/opieftp.ts \
	 ../../../i18n/ko/opieftp.ts \
	 ../../../i18n/no/opieftp.ts \
	 ../../../i18n/pl/opieftp.ts \
	 ../../../i18n/pt/opieftp.ts \
	 ../../../i18n/pt_BR/opieftp.ts \
	 ../../../i18n/sl/opieftp.ts \
	 ../../../i18n/zh_CN/opieftp.ts \
	 ../../../i18n/zh_TW/opieftp.ts
