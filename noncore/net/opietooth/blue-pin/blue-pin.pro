TEMPLATE        = app
#CONFIG         = qt warn_on debug
CONFIG          = qt warn_on release
HEADERS         = pindlg.h 
SOURCES         = main.cc pindlg.cc
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe 
INTERFACES      = pindlgbase.ui
TARGET          = $(OPIEDIR)bin/bluepin

