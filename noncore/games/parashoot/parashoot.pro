CONFIG		+= qt warn_on release quick-app
HEADERS		= interface.h man.h cannon.h base.h bullet.h helicopter.h
SOURCES		= main.cpp interface.cpp man.cpp cannon.cpp base.cpp bullet.cpp helicopter.cpp
TARGET		= parashoot 
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe



include ( $(OPIEDIR)/include.pro )
