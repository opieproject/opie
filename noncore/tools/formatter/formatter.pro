CONFIG    += qt warn_on release quick-app
HEADERS   = formatter.h inputDialog.h output.h
SOURCES   = formatter.cpp inputDialog.cpp output.cpp main.cpp
TARGET    = formatter
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe

include ( $(OPIEDIR)/include.pro )
