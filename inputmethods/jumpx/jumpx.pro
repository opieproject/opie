TEMPLATE        = lib
CONFIG         += qt warn_on release
HEADERS         = keyboard.h \
	          keyboardimpl.h
SOURCES         = keyboard.cpp \
                  keyboardimpl.cpp
TARGET          = qjumpx
DESTDIR         = ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS           += -lqpe
VERSION         = 1.0.0


TRANSLATIONS = ../../i18n/de/libqjumpx.ts
TRANSLATIONS += ../../i18n/en/libqjumpx.ts
TRANSLATIONS += ../../i18n/es/libqjumpx.ts
TRANSLATIONS += ../../i18n/fr/libqjumpx.ts
TRANSLATIONS += ../../i18n/hu/libqjumpx.ts
TRANSLATIONS += ../../i18n/ja/libqjumpx.ts
TRANSLATIONS += ../../i18n/ko/libqjumpx.ts
TRANSLATIONS += ../../i18n/no/libqjumpx.ts
TRANSLATIONS += ../../i18n/pl/libqjumpx.ts
TRANSLATIONS += ../../i18n/pt/libqjumpx.ts
TRANSLATIONS += ../../i18n/pt_BR/libqjumpx.ts
TRANSLATIONS += ../../i18n/sl/libqjumpx.ts
TRANSLATIONS += ../../i18n/zh_CN/libqjumpx.ts
TRANSLATIONS += ../../i18n/zh_TW/libqjumpx.ts

