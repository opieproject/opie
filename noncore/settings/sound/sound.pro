TEMPLATE  = app
CONFIG    += qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = soundsettings.h soundsettingsbase.h
SOURCES   = soundsettings.cpp soundsettingsbase.cpp main.cpp
#INTERFACES = soundsettingsbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
TARGET    = sound

TRANSLATIONS = ../../../i18n/de/sound.ts \
	 ../../../i18n/en/sound.ts \
	 ../../../i18n/es/sound.ts \
	 ../../../i18n/fr/sound.ts \
	 ../../../i18n/hu/sound.ts \
	 ../../../i18n/ja/sound.ts \
	 ../../../i18n/ko/sound.ts \
	 ../../../i18n/no/sound.ts \
	 ../../../i18n/pl/sound.ts \
	 ../../../i18n/pt/sound.ts \
	 ../../../i18n/pt_BR/sound.ts \
	 ../../../i18n/sl/sound.ts \
	 ../../../i18n/zh_CN/sound.ts \
	 ../../../i18n/zh_TW/sound.ts
