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

TRANSLATIONS = ../../i18n/de/libqkjumpx.ts \
	 ../../i18n/da/libqkjumpx.ts \
	 ../../i18n/xx/libqkjumpx.ts \
	 ../../i18n/en/libqkjumpx.ts \
	 ../../i18n/es/libqkjumpx.ts \
	 ../../i18n/fr/libqkjumpx.ts \
	 ../../i18n/hu/libqkjumpx.ts \
	 ../../i18n/ja/libqkjumpx.ts \
	 ../../i18n/ko/libqkjumpx.ts \
	 ../../i18n/no/libqkjumpx.ts \
	 ../../i18n/pl/libqkjumpx.ts \
	 ../../i18n/pt/libqkjumpx.ts \
	 ../../i18n/pt_BR/libqkjumpx.ts \
	 ../../i18n/sl/libqkjumpx.ts \
	 ../../i18n/zh_CN/libqkjumpx.ts \
	 ../../i18n/zh_TW/libqkjumpx.ts
