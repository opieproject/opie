TEMPLATE        = lib
CONFIG         += qt warn_on release
HEADERS         = keyboard.h \
	          keyboardimpl.h
SOURCES         = keyboard.cpp \
                  keyboardimpl.cpp
TARGET          = qkjumpx
DESTDIR         = ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS           += -lqpe
VERSION         = 1.0.0


TRANSLATIONS = ../../i18n/de/libqkjumpx.ts
TRANSLATIONS += ../../i18n/en/libqkjumpx.ts
TRANSLATIONS += ../../i18n/es/libqkjumpx.ts
TRANSLATIONS += ../../i18n/fr/libqkjumpx.ts
TRANSLATIONS += ../../i18n/hu/libqkjumpx.ts
TRANSLATIONS += ../../i18n/ja/libqkjumpx.ts
TRANSLATIONS += ../../i18n/ko/libqkjumpx.ts
TRANSLATIONS += ../../i18n/no/libqkjumpx.ts
TRANSLATIONS += ../../i18n/pl/libqkjumpx.ts
TRANSLATIONS += ../../i18n/pt/libqkjumpx.ts
TRANSLATIONS += ../../i18n/pt_BR/libqkjumpx.ts
TRANSLATIONS += ../../i18n/sl/libqkjumpx.ts
TRANSLATIONS += ../../i18n/zh_CN/libqkjumpx.ts
TRANSLATIONS += ../../i18n/zh_TW/libqkjumpx.ts

