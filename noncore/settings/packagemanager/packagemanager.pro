CONFIG       = qt warn_on release quick-app

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

TARGET       = packagemanager
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopie

include ( $(OPIEDIR)/include.pro )

