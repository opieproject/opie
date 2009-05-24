CONFIG        += qt warn_on quick-app
HEADERS        = profileedit.h profilelist.h kbddhandler.h
SOURCES        = profileedit.cpp profilelist.cpp main.cpp kbddhandler.cpp
INTERFACES     = profileeditbase.ui profilelistbase.ui
INCLUDEPATH   += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS          += -lqpe -lopiecore2
TARGET         = kbddcontrol

include( $(OPIEDIR)/include.pro )
