TEMPLATE = lib
CONFIG -= moc
CONFIG += qt release

HEADERS = stocktickerplugin.h stocktickerpluginimpl.h stocktickerpluginwidget.h stocktickerconfig.h \
      ../libstocks/csv.h \
      ../libstocks/http.h \
      ../libstocks/lists.h \
      ../libstocks/stocks.h
SOURCES = stocktickerplugin.cpp stocktickerpluginimpl.cpp stocktickerpluginwidget.cpp stocktickerconfig.cpp \
      ../libstocks/csv.c \
      ../libstocks/currency.c \
      ../libstocks/history.c \
      ../libstocks/http.c \
      ../libstocks/lists.c \
      ../libstocks/stocks.c

INCLUDEPATH     += $(OPIEDIR)/include \
    ../ ../library
DEPENDPATH      += $(OPIEDIR)/include \
    ../ ../library

LIBS+= -lqpe -lopie -lpthread
TMAKE_CFLAGS += -D__UNIX__

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaystocktickerplugin

TRANSLATIONS = ../../../../i18n/de/libtodaystocktickerplugin.ts \
         ../../../../i18n/xx/libtodaystocktickerplugin.ts \
         ../../../../i18n/en/libtodaystocktickerplugin.ts \
         ../../../../i18n/es/libtodaystocktickerplugin.ts \
         ../../../../i18n/fr/libtodaystocktickerplugin.ts \
         ../../../../i18n/hu/libtodaystocktickerplugin.ts \
         ../../../../i18n/ja/libtodaystocktickerplugin.ts \
         ../../../../i18n/ko/libtodaystocktickerplugin.ts \
         ../../../../i18n/no/libtodaystocktickerplugin.ts \
         ../../../../i18n/pl/libtodaystocktickerplugin.ts \
         ../../../../i18n/pt/libtodaystocktickerplugin.ts \
         ../../../../i18n/pt_BR/libtodaystocktickerplugin.ts \
         ../../../../i18n/sl/libtodaystocktickerplugin.ts \
         ../../../../i18n/zh_CN/libtodaystocktickerplugin.ts \
         ../../../../i18n/zh_TW/libtodaystocktickerplugin.ts \
         ../../../../i18n/it/libtodaystocktickerplugin.ts \
         ../../../../i18n/da/libtodaystocktickerplugin.ts

include ( $(OPIEDIR)/include.pro )
