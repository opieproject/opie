CONFIG      += qt warn_on release quick-app
HEADERS      = launchersettings.h \
               tabssettings.h \
               taskbarsettings.h \
               menusettings.h \
               tabconfig.h \
               tabdialog.h

SOURCES      = main.cpp \
               launchersettings.cpp \
               tabssettings.cpp \
               taskbarsettings.cpp \
               menusettings.cpp \
               tabdialog.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie
TARGET       = launchersettings




include ( $(OPIEDIR)/include.pro )
