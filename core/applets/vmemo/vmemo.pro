TEMPLATE  = lib
CONFIG    += qt warn_on release
HEADERS =   vmemo.h vmemoimpl.h adpcm.h
SOURCES =   vmemo.cpp vmemoimpl.cpp adpcm.c
TARGET    = vmemoapplet
DESTDIR   =$(OPIEDIR)/plugins/applets
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe
VERSION   = 1.0.0

TRANSLATIONS = ../../../i18n/de/libvmemoapplet.ts \
   ../../../i18n/en/libvmemoapplet.ts \
   ../../../i18n/es/libvmemoapplet.ts \
   ../../../i18n/fr/libvmemoapplet.ts \
   ../../../i18n/hu/libvmemoapplet.ts \
   ../../../i18n/ja/libvmemoapplet.ts \
   ../../../i18n/ko/libvmemoapplet.ts \
   ../../../i18n/no/libvmemoapplet.ts \
   ../../../i18n/pl/libvmemoapplet.ts \
   ../../../i18n/pt/libvmemoapplet.ts \
   ../../../i18n/pt_BR/libvmemoapplet.ts \
   ../../../i18n/sl/libvmemoapplet.ts \
   ../../../i18n/zh_CN/libvmemoapplet.ts \
   ../../../i18n/zh_TW/libvmemoapplet.ts
