TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= calcdisplay.h    \
                calckeypad.h
SOURCES		= calcdisplay.cpp  \
                calckeypad.cpp \
                main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopie
DESTDIR		= ${OPIEDIR}/bin
INTERFACES	= 

include ( $(OPIEDIR)/include.pro )
