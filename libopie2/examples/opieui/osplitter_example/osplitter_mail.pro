CONFIG += qt warn_on
TEMPLATE = app
TARGET = osplitter-mail

INCLUDEPATH +=  $(OPIEDIR)/include
DEPENDSPATH +=  $(OPIEDIR)/include

HEADERS = osplitter_mail.h
SOURCES = osplitter_mail.cpp

LIBS += -lqpe -lopieui2
include ( $(OPIEDIR)/include.pro )
