CONFIG    = qt warn_on release quick-app
HEADERS   = appearance.h editScheme.h sample.h
SOURCES   = appearance.cpp editScheme.cpp main.cpp sample.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += ../$(OPIEDIR)/include
LIBS            += -lqpe -lopie
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

TARGET 		= appearance

include ( $(OPIEDIR)/include.pro )
