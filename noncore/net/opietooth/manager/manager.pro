CONFIG          = qt warn_on release quick-app
HEADERS         = btconnectionitem.h btdeviceitem.h \
                  btserviceitem.h stdpopups.h \ 
		  popuphelper.h bluebase.h \ 
		  scandialog.h btlistitem.h \ 
		  hciconfwrapper.h bticonloader.h \
		  pppdialog.h obexdialog.h \
		rfcommassigndialogimpl.h rfcommassigndialogitem.h \
		  devicehandler.h rfcpopup.h obexpopup.h \
		  rfcommhelper.h panpopup.h dunpopup.h rfcommconfhandler.h
		  
SOURCES         = btconnectionitem.cpp btdeviceitem.cpp \ 
	          btserviceitem.cpp stdpopups.cpp \
		  popuphelper.cpp main.cpp \
		  bluebase.cpp scandialog.cpp \ 
		  btlistitem.cpp hciconfwrapper.cpp \ 
		  bticonloader.cpp pppdialog.cpp \
		rfcommassigndialogimpl.cpp rfcommassigndialogitem.cpp \
		  obexdialog.cpp devicehandler.cpp \
		  rfcpopup.cpp obexpopup.cpp \
		  rfcommhelper.cpp panpopup.cpp dunpopup.cpp rfcommconfhandler.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH     += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopietooth -lopiecore2
INTERFACES      = bluetoothbase.ui  devicedialog.ui rfcommassigndialogbase.ui rfcommdialogitembase.ui

TARGET 		= bluetooth-manager

include ( $(OPIEDIR)/include.pro )
