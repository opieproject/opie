TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= 
SOURCES		= zeilenweise.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiedb2
TARGET		= zeilenweise




include ( $(OPIEDIR)/include.pro )
