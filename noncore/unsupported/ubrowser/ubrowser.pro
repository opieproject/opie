CONFIG		+=qt warn_on  quick-app
HEADERS		= mainview.h httpfactory.h
SOURCES		= main.cpp mainview.cpp httpfactory.cpp
INCLUDEPATH	+=../../../include
DEPENDPATH	+=../../../include
LIBS		+= -lqpe
TARGET          = ubrowser



include ( $(OPIEDIR)/include.pro )
