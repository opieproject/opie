CONFIG       = qt warn_on release quick-app
//TEMPLATE     = app
//CONFIG      += qte warn_on debug
//DESTDIR      = $(OPIEDIR)/bin

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
               promptdlg.h

DEFINES     += IPKG_LIB
DEFINES     += HAVE_MKDTEMP
TARGET       = packagemanager
INCLUDEPATH += $(OPIEDIR)/include $(IPKGDIR)
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie -lipkg

include ( $(OPIEDIR)/include.pro )

