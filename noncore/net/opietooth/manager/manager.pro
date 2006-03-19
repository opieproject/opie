CONFIG          = qt warn_on  quick-app
HEADERS         = btconnectionitem.h btdeviceitem.h \
                  btserviceitem.h stdpopups.h \ 
		  popuphelper.h bluebase.h \ 
		  scandialog.h btlistitem.h filistitem.h \ 
		  hciconfwrapper.h bticonloader.h \
		  pppdialog.h obexdialog.h obexftpdialog.h \
		rfcommassigndialogimpl.h rfcommassigndialogitem.h \
		  devicehandler.h rfcpopup.h obexpopup.h obexftpopup.h \
		  rfcommhelper.h panpopup.h dunpopup.h rfcommconfhandler.h
		  
SOURCES         = btconnectionitem.cpp btdeviceitem.cpp \ 
	          btserviceitem.cpp filelistitem.cpp stdpopups.cpp \
		  popuphelper.cpp main.cpp \
		  bluebase.cpp scandialog.cpp \ 
		  btlistitem.cpp hciconfwrapper.cpp \ 
		  bticonloader.cpp pppdialog.cpp \
		rfcommassigndialogimpl.cpp rfcommassigndialogitem.cpp \
		  obexdialog.cpp devicehandler.cpp \
		  rfcpopup.cpp obexpopup.cpp obexftpopup.cpp obexftpdialog.cpp \
		  rfcommhelper.cpp panpopup.cpp dunpopup.cpp rfcommconfhandler.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH     += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lbluetooth -lopietooth1 -lopiecore2 -lopieui2 -lopenobex 
INTERFACES      = bluetoothbase.ui  devicedialog.ui rfcommassigndialogbase.ui \
                  rfcommdialogitembase.ui obexftpdialogbase.ui

TARGET 		= bluetooth-manager

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBBLUEZ_INC_DIR ) {
    INCLUDEPATH += $$LIBBLUEZ_INC_DIR/obexftp
}

!isEmpty( LIBBLUEZ_LIB_DIR ) {
    LIBS += $$LIBBLUEZ_LIB_DIR/libobexftp.a
    LIBS += $$LIBBLUEZ_LIB_DIR/libmulticobex.a
    LIBS += $$LIBBLUEZ_LIB_DIR/libbfb.a
}