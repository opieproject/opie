CONFIG		= qt warn_on release quick-app
HEADERS		= fifteen.h
SOURCES		= fifteen.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
TARGET		= fifteen

include ( $(OPIEDIR)/include.pro )
