CONFIG        = qt warn_on  quick-app
HEADERS        = oxygen.h \
            kmolcalc.h \
            kmolelements.h \
            calcdlgui.h \
            dataTable.h \
            psewidget.h \
            oxyframe.h \
            datawidgetui.h

SOURCES        = main.cpp \
          oxygen.cpp \
          kmolcalc.cpp \
          calcdlgui.cpp \
          kmolelements.cpp \
          dataTable.cpp \
          psewidget.cpp \
          oxyframe.cpp \
          datawidgetui.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2
INTERFACES    = calcdlg.ui

TARGET    = oxygen

include ( $(OPIEDIR)/include.pro )
