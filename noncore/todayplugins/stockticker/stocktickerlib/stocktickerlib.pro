TEMPLATE = lib
CONFIG -= moc
CONFIG += qt debug

HEADERS = stocktickerplugin.h stocktickeruginimpl.h stocktickerpluginwidget.h stocktickerconfig.h \
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
