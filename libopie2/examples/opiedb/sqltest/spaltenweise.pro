TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= 
SOURCES		= spaltenweise.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiedb2
TARGET		= spaltenweise




include ( $(OPIEDIR)/include.pro )
