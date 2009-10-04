CONFIG       = qt warn_on  quick-app

SOURCES      = opackage.cpp        \
               oconfitem.cpp       \
               oipkg.cpp           \
               oipkgconfigdlg.cpp  \
               opackagemanager.cpp \
               mainwindow.cpp      \
               installdlg.cpp      \
               packageinfodlg.cpp  \
               filterdlg.cpp       \
               promptdlg.cpp       \
               entrydlg.cpp        \
               main.cpp
HEADERS      = opackage.h        \
               oconfitem.h       \
               oipkg.h           \
               oipkgconfigdlg.h  \
               opackagemanager.h \
               mainwindow.h      \
               installdlg.h      \
               packageinfodlg.h  \
               filterdlg.h       \
               promptdlg.h       \
               entrydlg.h

DEFINES     += IPKG_LIB
DEFINES     += HAVE_MKDTEMP
TARGET       = packagemanager
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2

CONFTEST = $$system( echo $CONFIG_LIBIPK_DEP )
contains( CONFTEST, y ) {
	LIBS += -lipkg
}

CONFTEST = $$system( echo $CONFIG_LIBOPK_DEP )
contains( CONFTEST, y ) {
	LIBS += -lopkg
	DEFINES += USE_LIBOPKG
}

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBIPK_INC_DIR ) {
    INCLUDEPATH = $$LIBIPK_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBIPK_LIB_DIR ) {
    LIBS = -L$$LIBIPK_LIB_DIR $$LIBS
}

