CONFIG		= qt warn_on quick-app
HEADERS		= KHCWidget.h \
		  KeyNames.h
SOURCES		= KHCWidget.cpp \
		  KeyNames.cpp \
		  khcmain.cpp
INTERFACES	= KHCWidgetBase.ui
DESTDIR			=	$(OPIEDIR)/bin
INCLUDEPATH += . $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS			+=	-lqpe

include( $(OPIEDIR)/include.pro )
