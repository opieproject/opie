CONFIG    = qt warn_on release quick-app
HEADERS   = clock.h setAlarm.h
SOURCES   = clock.cpp setAlarm.cpp \
      main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lpthread
TARGET    = clock

include ( $(OPIEDIR)/include.pro )
