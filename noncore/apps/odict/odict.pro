CONFIG		= qt warn_on release quick-app
HEADERS		= odict.h \
				searchmethoddlg.h \
				configdlg.h \
				dingwidget.h 

SOURCES		= main.cpp \
		  odict.cpp \
		  searchmethoddlg.cpp \
		  configdlg.cpp \
		  dingwidget.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe -lstdc++ -lopie
TARGET 		= odict

include ( $(OPIEDIR)/include.pro )
