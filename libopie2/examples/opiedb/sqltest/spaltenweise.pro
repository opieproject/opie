TEMPLATE	= app
CONFIG		= qt warn_on 
HEADERS		= 
SOURCES		= spaltenweise.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiedb2
TARGET		= spaltenweise




include ( $(OPIEDIR)/include.pro )
