#CONFIG		= qt warn_on release quick-app
CONFIG		= qt warn_on debug  quick-app
HEADERS	= addressbook.h \
                  contacteditor.h \
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
LIBS            += -lqpe -lopiecore2 -lopieui2 -lopiepim2 -lopiedb2

include ( $(OPIEDIR)/include.pro )
