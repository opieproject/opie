DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          += qte warn_on quick_app
INTERFACES=camerabase.ui camerasettings.ui
HEADERS		= mainwindow.h \
                  image.h \
		    thumbbutton.h \
		    videocaptureview.h
SOURCES		= mainwindow.cpp\
                  image.cpp \
		    videocaptureview.cpp \
		    main.cpp

TARGET		= camera2
DEFINES += HAVE_VIDEO4LINUX

LIBS            += -lopiecore2 -lopieui2
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
contains(CONFIG,quick-app) {
 DESTDIR = $(OPIEDIR)/bin
 DEFINES += NOQUICKLAUNCH
}


include( $(OPIEDIR)/include.pro )

