CONFIG		+= qt warn_on release quick-app
HEADERS		= snake.h target.h obstacle.h interface.h codes.h
SOURCES		= snake.cpp target.cpp obstacle.cpp interface.cpp main.cpp
TARGET		= snake
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe


include ( $(OPIEDIR)/include.pro )
