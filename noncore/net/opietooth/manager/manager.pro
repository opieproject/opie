TEMPLATE        = app
#CONFIG         = qt warn_on debug
CONFIG          = qt warn_on release
HEADERS         = bluebase.h scandialog.h 
SOURCES         = main.cpp bluebase.cpp scandialog.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopietooth -lopie
INTERFACES      = bluetoothbase.ui  devicedialog.ui  
TARGET          = $(OPIEDIR)/bin/bluetooth-manager

