TEMPLATE	= app
CONFIG		= qt warn_on 
HEADERS		= 
SOURCES		= zeilenweise.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiedb2
TARGET		= zeilenweise




include ( $(OPIEDIR)/include.pro )
