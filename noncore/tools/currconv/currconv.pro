CONFIG		= qt warn_on  quick-app
HEADERS		= calcdisplay.h    \
                calckeypad.h
SOURCES		= calcdisplay.cpp  \
                calckeypad.cpp \
                main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES	= 
TARGET          = currconv

include( $(OPIEDIR)/include.pro )
