TEMPLATE        = app
#CONFIG         = qt warn_on debug
CONFIG          = qt warn_on release
HEADERS         = bluebase.h 
SOURCES         = main.cpp bluebase.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH   += $(OPIEDIR)/noncore/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopietooth
INTERFACES      = bluetoothbase.ui  devicedialog.ui  scandialog.ui
TARGET          = $(OPIEDIR)/bin/bluetooth-manager

