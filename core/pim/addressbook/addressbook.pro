#CONFIG		= qt warn_on release
CONFIG		= qt warn_on quick-app

HEADERS	= addressbook.h \
                  contacteditor.h \
                  ocontactfields.h \
		  ablabel.h \
		  abtable.h \
		  picker.h \
                  ofloatbar.h \
                  configdlg.h \
                  abconfig.h \
                  abview.h
SOURCES	= main.cpp \
		  addressbook.cpp \
		  contacteditor.cpp \
                  ocontactfields.cpp \
		  ablabel.cpp \
		  abtable.cpp \
		  picker.cpp \
                  configdlg.cpp \
                  abconfig.cpp \
                  abview.cpp

INTERFACES	= configdlg_base.ui
TARGET		= addressbook
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )
