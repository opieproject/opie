TEMPLATE	=	lib
CONFIG		+=	qt warn_on release
HEADERS		+=	bend.h \
			bendimpl.h
SOURCES		+=	bend.cpp \
			bendimpl.cpp
INCLUDEPATH	+=	$(OPIEDIR)/include ../libmail
LIBS		+=	-lcoremail -lqpe
TARGET		=	bend
DESTDIR		+=	$(OPIEDIR)/plugins/applets/

include ( $(OPIEDIR)/include.pro )
