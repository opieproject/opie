TEMPLATE       = app
CONFIG         = qt warn_on quick-app
DESTDIR        = $(OPIEDIR)/bin
HEADERS        = tinykate.h
SOURCES        = tinykate.cpp main.cpp
INCLUDEPATH    = $(OPIEDIR)/include ../libkate \
                 ../libkate/ktexteditor ../libkate/document ../libkate/microkde ../libkate/qt3back ../libkate/view ../libkate/interfaces
DEPENDPATH     += $(OPIEDIR)/include
LIBS           += -lqpe -lkate -lopiecore2 -lopieui2
TARGET         = tinykate

include ( $(OPIEDIR)/include.pro )
