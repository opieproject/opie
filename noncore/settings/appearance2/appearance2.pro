TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = appearance.h editScheme.h sample.h
SOURCES   = appearance.cpp editScheme.cpp main.cpp sample.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET    = appearance
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

