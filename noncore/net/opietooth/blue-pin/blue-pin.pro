CONFIG          = qt warn_on release quick-app
HEADERS         = pindlg.h 
SOURCES         = main.cc pindlg.cc
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH    += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe 
INTERFACES      = pindlgbase.ui
TARGET		= bluepin

include ( $(OPIEDIR)/include.pro )
