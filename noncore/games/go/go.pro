CONFIG		= qt warn_on  quick-app
HEADERS		= amigo.h \
		  go.h \
		  goplayutils.h \
		  gowidget.h
SOURCES		= amigo.c \
		goplayer.c \
		goplayutils.c \
		killable.c \
		gowidget.cpp \
		main.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET		= go



include ( $(OPIEDIR)/include.pro )
