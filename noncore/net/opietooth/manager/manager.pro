TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS		= bluemanager.h  
SOURCES		= main.cpp bluemanager.cpp 

INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe
INTERFACES      = bluetoothbase.ui devicedialog.ui scandialog.ui
TARGET		= bluetooth-manager
DESTDIR		= $(OPIEDIR)/bin
