TEMPLATE    = app
CONFIG        = qt warn_on  quick-app
DESTDIR        = $(OPIEDIR)/bin
HEADERS        = tinykate.h
SOURCES        = tinykate.cpp main.cpp
INTERFACES    =
INCLUDEPATH    += $(OPIEDIR)/include \
            $(OPIEDIR)/noncore/apps/tinykate/libkate \
            $(OPIEDIR)/noncore/apps/tinykate/libkate/microkde \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/document \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/view \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/interfaces \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/ktexteditor \
                        $(OPIEDIR)/noncore/apps/tinykate/libkate/qt3back

DEPENDPATH    += $(OPIEDIR)/include
LIBS            += -lqpe -ltinykate -lopiecore2 -lopieui2
TARGET        = kate



include ( $(OPIEDIR)/include.pro )
