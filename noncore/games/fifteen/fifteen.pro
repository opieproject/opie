CONFIG		= qt warn_on  quick-app
HEADERS		= fifteen.h
SOURCES		= fifteen.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET		= fifteen

include ( $(OPIEDIR)/include.pro )
