CONFIG        = qt warn_on release quick-app
HEADERS        = mainwindow.h          \
                timetabwidget.h     \
                formattabwidget.h   \
                settingstabwidget.h \
                predicttabwidget.h  \
                ntptabwidget.h
SOURCES        = main.cpp                \
                mainwindow.cpp        \
                timetabwidget.cpp     \
                formattabwidget.cpp   \
                settingstabwidget.cpp \
                predicttabwidget.cpp  \
                ntptabwidget.cpp
INCLUDEPATH    += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2

TARGET        = systemtime

include ( $(OPIEDIR)/include.pro )
