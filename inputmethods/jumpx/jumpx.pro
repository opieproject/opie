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

TRANSLATIONS = ../../i18n/de/libqjumpx.ts \
	 ../../i18n/en/libqjumpx.ts \
	 ../../i18n/es/libqjumpx.ts \
	 ../../i18n/fr/libqjumpx.ts \
	 ../../i18n/hu/libqjumpx.ts \
	 ../../i18n/ja/libqjumpx.ts \
	 ../../i18n/ko/libqjumpx.ts \
	 ../../i18n/no/libqjumpx.ts \
	 ../../i18n/pl/libqjumpx.ts \
	 ../../i18n/pt/libqjumpx.ts \
	 ../../i18n/pt_BR/libqjumpx.ts \
	 ../../i18n/sl/libqjumpx.ts \
	 ../../i18n/zh_CN/libqjumpx.ts \
	 ../../i18n/zh_TW/libqjumpx.ts
