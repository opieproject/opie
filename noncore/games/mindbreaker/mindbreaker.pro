CONFIG          += qt warn_on release quick-app
HEADERS		= mindbreaker.h
SOURCES		= main.cpp \
              mindbreaker.cpp
TARGET          = mindbreaker
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe


include ( $(OPIEDIR)/include.pro )
