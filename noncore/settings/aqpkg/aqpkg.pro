CONFIG		= qt warn_on release quick-app
HEADERS		= global.h \
		  mainwin.h \
		  datamgr.h \
		  settingsimpl.h \
		  ipkg.h \
		  packagewin.h \
		  package.h \
		  installdlgimpl.h \
		  instoptionsimpl.h \
		  destination.h \
		  utils.h \
		  server.h \
		  letterpushbutton.h \
		  inputdlg.h \
		  categoryfilterimpl.h
SOURCES		= mainwin.cpp \
		  datamgr.cpp \
		  mem.cpp \
		  settingsimpl.cpp \
		  ipkg.cpp \
		  main.cpp \
		  packagewin.cpp \
		  package.cpp \
		  installdlgimpl.cpp \
		  instoptionsimpl.cpp \
		  destination.cpp \
		  utils.cpp \
		  server.cpp \
		  letterpushbutton.cpp \
		  inputdlg.cpp \
		  version.cpp \
		  categoryfilterimpl.cpp
TARGET		= aqpkg
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie -lstdc++

include ( $(OPIEDIR)/include.pro )

