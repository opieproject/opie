TEMPLATE  = app
CONFIG    = qt warn_on release
HEADERS   = inputDialog.h helpwindow.h
SOURCES   = inputDialog.cpp helpwindow.cpp main.cpp
INTERFACES  = 
TARGET    = stockticker
INCLUDEPATH     += $(OPIEDIR)/include 
DEPENDPATH      += $(OPIEDIR)/include
LIBS+= -lqpe -lopie
DESTDIR = $(OPIEDIR)/bin

TRANSLATIONS = ../../../../i18n/de/stockticker.ts \
         ../../../../i18n/xx/stockticker.ts \
         ../../../../i18n/en/stockticker.ts \
         ../../../../i18n/es/stockticker.ts \
         ../../../../i18n/fr/stockticker.ts \
         ../../../../i18n/hu/stockticker.ts \
         ../../../../i18n/ja/stockticker.ts \
         ../../../../i18n/ko/stockticker.ts \
         ../../../../i18n/no/stockticker.ts \
         ../../../../i18n/pl/stockticker.ts \
         ../../../../i18n/pt/stockticker.ts \
         ../../../../i18n/pt_BR/stockticker.ts \
         ../../../../i18n/sl/stockticker.ts \
         ../../../../i18n/zh_CN/stockticker.ts \
         ../../../../i18n/zh_TW/stockticker.ts \
         ../../../../i18n/it/stockticker.ts \
         ../../../../i18n/da/stockticker.ts

include ( $(OPIEDIR)/include.pro )
