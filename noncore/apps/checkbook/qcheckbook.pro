TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= config.h \
		  qcheckbook.h \
		  qcheckdetails.h \
		  qcheckentry.h \
		  qcheckgraph.h \
		  qcheckmainmenu.h \
		  qcheckname.h \
		  qcheckview.h \
		  qrestrictedcombo.h \
		  qrestrictedline.h
SOURCES		= config.cpp \
		  main.cpp \
		  qcheckbook.cpp \
		  qcheckdetails.cpp \
		  qcheckentry.cpp \
		  qcheckgraph.cpp \
		  qcheckmainmenu.cpp \
		  qcheckname.cpp \
		  qcheckview.cpp \
		  qrestrictedcombo.cpp \
		  qrestrictedline.cpp
INTERFACES	= qcheckdetailsbase.ui \
		  qcheckentrybase.ui \
		  qcheckgraphbase.ui \
		  qcheckmmbase.ui \
		  qchecknamebase.ui \
		  qcheckviewbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe
TARGET 		= $(OPIEDIR)/bin/checkbook