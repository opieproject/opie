TEMPLATE      = lib
CONFIG       += qt plugn warn_on release
HEADERS       = rotate.h
SOURCES       = rotate.cpp
TARGET        = rotateapplet
DESTDIR       = $(OPIEDIR)/plugins/applets
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe
VERSION       = 1.0.0

TRANSLATIONS = ../../../i18n/de/librotateapplet.ts \
	 ../../../i18n/nl/librotateapplet.ts \
         ../../../i18n/da/librotateapplet.ts \
         ../../../i18n/xx/librotateapplet.ts \
         ../../../i18n/en/librotateapplet.ts \
         ../../../i18n/es/librotateapplet.ts \
         ../../../i18n/fr/librotateapplet.ts \
         ../../../i18n/hu/librotateapplet.ts \
         ../../../i18n/ja/librotateapplet.ts \
         ../../../i18n/ko/librotateapplet.ts \
         ../../../i18n/no/librotateapplet.ts \
         ../../../i18n/pl/librotateapplet.ts \
         ../../../i18n/pt/librotateapplet.ts \
         ../../../i18n/pt_BR/librotateapplet.ts \
         ../../../i18n/sl/librotateapplet.ts \
         ../../../i18n/zh_CN/librotateapplet.ts \
         ../../../i18n/zh_TW/librotateapplet.ts

include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/applets
