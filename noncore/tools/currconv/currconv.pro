CONFIG		= qt warn_on  quick-app
HEADERS		= calcdisplay.h    \
                calckeypad.h   \
                json.h
SOURCES		= calcdisplay.cpp  \
                calckeypad.cpp \
                currency.cpp \
                main.cpp \
                json.c
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES	= 
TARGET          = currconv

include( $(OPIEDIR)/include.pro )
