DESTDIR		= $(OPIEDIR)/bin
TEMPLATE	= app
#CONFIG		= qt warn_on release
CONFIG		= qt warn_on debug
HEADERS		= mainwindow.h \
		pksettings.h \
		pmipkg.h \
		utils.h \
		packagelistitem.h \
		packagelistremote.h \
		packagelist.h \
		packagelistlocal.h \
		packagelistview.h \
		package.h
SOURCES		= main.cpp \
		mainwindow.cpp \
		utils.cpp \
		packagelistview.cpp \
		packagelistremote.cpp \
		packagelistlocal.cpp \
		pksettings.cpp \
		pmipkg.cpp \
		packagelistitem.cpp \
		packagelist.cpp \
		package.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/ioclude
LIBS            += -lqpe 
INTERFACES	= runwindow.ui \
		pksettingsbase.ui
TARGET		= oipkg

