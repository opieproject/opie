TEMPLATE = lib
CONFIG += qt plugin 

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

LIBS    += -lqpe -lopiecore2 -lopieui2 -lopiepim2 -lpthread
DEFINES += __UNIX__

DESTDIR = $(OPIEDIR)/plugins/today
TARGET = todaystocktickerplugin

include ( $(OPIEDIR)/include.pro )
