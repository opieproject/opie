CONFIG    = qt warn_on  quick-app
HEADERS   = clock.h setAlarm.h
SOURCES   = clock.cpp setAlarm.cpp \
      main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lpthread
TARGET    = clock

include ( $(OPIEDIR)/include.pro )
