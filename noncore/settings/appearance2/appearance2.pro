CONFIG    = qt warn_on release quick-app
HEADERS   = appearance.h editScheme.h sample.h
SOURCES   = appearance.cpp editScheme.cpp main.cpp sample.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lqtaux2

TARGET         = appearance

include ( $(OPIEDIR)/include.pro )
