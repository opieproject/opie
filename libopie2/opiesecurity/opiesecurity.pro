TEMPLATE    = lib
CONFIG      += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = multiauthcommon.h \
              multiauthmainwindow.h \
              multiauthconfigwidget.h \
              multiauthplugininterface.h
SOURCES     = multiauthcommon.cpp \
              multiauthmainwindow.cpp
TARGET      = opiesecurity2
VERSION     = 0.0.2
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

include ( $(OPIEDIR)/include.pro )
