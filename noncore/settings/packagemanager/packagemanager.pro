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
INCLUDEPATH += $(OPIEDIR)/include $(IPKGDIR)
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2 -lipkg

include ( $(OPIEDIR)/include.pro )

