CONFIG    = qt warn_on  quick-app
HEADERS   = clock.h analogclock.h
SOURCES   = clock.cpp analogclock.cpp \
      main.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lpthread
TARGET    = clock


INTERFACES	= clockbase.ui \
		alarmdlgbase.ui

include ( $(OPIEDIR)/include.pro )
