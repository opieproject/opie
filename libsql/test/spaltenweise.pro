TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= 
SOURCES		= spaltenweise.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiesql
TARGET		= spaltenweise

