TEMPLATE        = app
CONFIG          += qt warn_on release
DESTDIR         = $(OPIEDIR)/bin
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe

include ( $(OPIEDIR)/include.pro )
