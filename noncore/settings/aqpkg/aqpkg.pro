TEMPLATE	= app
CONFIG		= qt warn_on debug
HEADERS		= global.h \
		  mainwin.h \
		  datamgr.h \
		  settingsimpl.h \
		  ipkg.h \
		  networkpkgmgr.h \
		  package.h \
		  progressdlg.h \
		  installdlgimpl.h \
		  instoptionsimpl.h \
		  destination.h \
		  utils.h \
		  server.h \
		  helpwindow.h \
		  letterpushbutton.h \
		  inputdlg.h
SOURCES		= mainwin.cpp \
		  datamgr.cpp \
		  mem.cpp \
		  settingsimpl.cpp \
		  ipkg.cpp \
		  networkpkgmgr.cpp \
		  main.cpp \
		  package.cpp \
		  progressdlg.cpp \
		  installdlgimpl.cpp \
		  instoptionsimpl.cpp \
		  destination.cpp \
		  utils.cpp \
		  server.cpp \
		  helpwindow.cpp \
		  letterpushbutton.cpp \
		  inputdlg.cpp
INTERFACES	= settings.ui \
		  install.ui \
		  instoptions.ui
TARGET		= aqpkg
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lstdc++

