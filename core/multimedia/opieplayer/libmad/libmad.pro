TEMPLATE  =   lib
CONFIG    +=  qt warn_on release
HEADERS   =   libmad_version.h fixed.h bit.h timer.h stream.h frame.h synth.h decoder.h \
        layer12.h layer3.h huffman.h libmad_global.h mad.h libmadplugin.h libmadpluginimpl.h
SOURCES         =   version.c fixed.c bit.c timer.c stream.c frame.c synth.c decoder.c \
        network.c layer12.c layer3.c huffman.c libmadplugin.cpp libmadpluginimpl.cpp
TARGET    =   madplugin
DESTDIR   =   $(OPIEDIR)/plugins/codecs
INCLUDEPATH += $(OPIEDIR)/include ..
DEPENDPATH      +=  ../$(OPIEDIR)/include ..
LIBS            +=  -lqpe -lm
VERSION   =   1.0.0

TRANSLATIONS    = ../../i18n/pt_BR/libmadplugin.ts
TRANSLATIONS   += ../../i18n/de/libmadplugin.ts
TRANSLATIONS   += ../../i18n/en/libmadplugin.ts
TRANSLATIONS   += ../../i18n/hu/libmadplugin.ts
TRANSLATIONS   += ../../i18n/sl/libmadplugin.ts
TRANSLATIONS   += ../../i18n/ja/libmadplugin.ts
TRANSLATIONS   += ../../i18n/pl/libmadplugin.ts
TRANSLATIONS   += ../../i18n/ko/libmadplugin.ts
TRANSLATIONS   += ../../i18n/no/libmadplugin.ts
TRANSLATIONS   += ../../i18n/zh_CN/libmadplugin.ts
TRANSLATIONS   += ../../i18n/zh_TW/libmadplugin.ts
TRANSLATIONS   += ../../i18n/fr/libmadplugin.ts
