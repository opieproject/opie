TEMPLATE  = app
CONFIG    += qt warn_on release
HEADERS   = tictac.h
SOURCES   = main.cpp \
      tictac.cpp
TARGET    = tictac
REQUIRES=medium-config
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe
DESTDIR   = $(OPIEDIR)/bin
