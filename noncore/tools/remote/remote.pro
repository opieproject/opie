TEMPLATE	=app
CONFIG		+=qt warn_on 
DESTDIR		=$(OPIEDIR)/bin
HEADERS		= remotetab.h learntab.h configtab.h topgroup.h dvdgroup.h channelgroup.h vcrgroup.h buttondialog.h topgroupconf.h dvdgroupconf.h channelgroupconf.h vcrgroupconf.h mainview.h recorddialog.h helptab.h
SOURCES		=remote.cpp remotetab.cpp learntab.cpp configtab.cpp topgroup.cpp dvdgroup.cpp channelgroup.cpp vcrgroup.cpp buttondialog.cpp topgroupconf.cpp dvdgroupconf.cpp channelgroupconf.cpp vcrgroupconf.cpp mainview.cpp recorddialog.cpp helptab.cpp
INCLUDEPATH	+=$(OPIEDIR)/include
DEPENDPATH	+=$(OPIEDIR)/include
LIBS		+=-lqpe -lopiecore2
TARGET          = remote

include ( $(OPIEDIR)/include.pro )
