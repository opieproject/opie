CONFIG      += qt warn_on  quick-app
HEADERS      = launchersettings.h \
               tabssettings.h \
               taskbarsettings.h \
               menusettings.h \
               inputmethodsettings.h \
               doctabsettings.h \
               tabconfig.h \
               tabdialog.h

SOURCES      = main.cpp \
               launchersettings.cpp \
               tabssettings.cpp \
               taskbarsettings.cpp \
               menusettings.cpp \
               inputmethodsettings.cpp \
               doctabsettings.cpp \
               tabdialog.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lqtaux2
TARGET       = launchersettings




include ( $(OPIEDIR)/include.pro )
