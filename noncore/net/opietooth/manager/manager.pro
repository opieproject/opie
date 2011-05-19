CONFIG          = qt warn_on  quick-app
HEADERS         = btconnectionitem.h btdeviceitem.h \
                  btserviceitem.h stdpopups.h \ 
		  popuphelper.h bluebase.h \ 
		  scandialog.h btlistitem.h filelistitem.h \
		  bticonloader.h \
		  pppdialog.h dundialog.h pandialog.h \
		  obexdialog.h obexftpdialog.h \
		  rfcommassigndialogimpl.h rfcommassigndialogitem.h \
		  rfcpopup.h obexpopup.h obexftpopup.h \
		  rfcommhelper.h panpopup.h dunpopup.h rfcommconfhandler.h \
		  servicesdialog.h btconfhandler.h hidpopup.h
		  
SOURCES         = btconnectionitem.cpp btdeviceitem.cpp \ 
	          btserviceitem.cpp filelistitem.cpp stdpopups.cpp \
		  popuphelper.cpp main.cpp \
		  bluebase.cpp scandialog.cpp \ 
		  btlistitem.cpp \
		  bticonloader.cpp pppdialog.cpp dundialog.cpp pandialog.cpp \
		  rfcommassigndialogimpl.cpp rfcommassigndialogitem.cpp \
		  obexdialog.cpp \
		  rfcpopup.cpp obexpopup.cpp obexftpopup.cpp obexftpdialog.cpp \
		  rfcommhelper.cpp panpopup.cpp dunpopup.cpp rfcommconfhandler.cpp \
		  servicesdialog.cpp btconfhandler.cpp hidpopup.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH     += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lbluetooth -lopietooth1 -lopiecore2 -lopieui2 -lopiebluez2 -lopenobex -lobexftp
INTERFACES      = bluetoothbase.ui  devicedialog.ui rfcommassigndialogbase.ui \
                  rfcommdialogitembase.ui obexftpdialogbase.ui \
                  servicesdialogbase.ui

TARGET 		= bluetooth-manager

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBOBEXFTP_INC_DIR ) {
    INCLUDEPATH += $$LIBOBEXFTP_INC_DIR
}

!isEmpty( LIBOBEXFTP_LIB_DIR ) {
    LIBS += $$LIBOBEXFTP_LIB_DIR/libobexftp.a
    LIBS += $$LIBOBEXFTP_LIB_DIR/libmulticobex.a
    LIBS += $$LIBOBEXFTP_LIB_DIR/libbfb.a
}
