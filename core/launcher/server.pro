TEMPLATE	= app

CONFIG		+= qt warn_on 

DESTDIR		= $$(OPIEDIR)/bin

HEADERS		+= server.h \
		  qrr.h \
		  serverinterface.h \
		  launchertab.h \
		  documentlist.h \
		  appicons.h \
		  taskbar.h \
                  runningappbar.h \
		  applauncher.h \
		  stabmon.h \
		  inputmethods.h \
		  systray.h \
		  wait.h \
		  shutdownimpl.h \
		  launcher.h \
		  launcherview.h \
		  $$(OPIEDIR)/core/apps/calibrate/calibrate.h \
		  startmenu.h \
		  transferserver.h \
		  qcopbridge.h \
		  packageslave.h \
		  irserver.h \
		  firstuse.h \
		  syncdialog.h \
		  serverapp.h \
		  qprocess.h \
		  screensaver.h \
		  $$(OPIEDIR)/noncore/settings/mediummount/mediumwidget.h \
                  mediadlg.h

SOURCES		+= server.cpp \
		  qrr.cpp \
		  serverinterface.cpp \
		  launchertab.cpp \
		  documentlist.cpp \
		  appicons.cpp \
		  taskbar.cpp \
                  runningappbar.cpp \
		  applauncher.cpp \
		  stabmon.cpp \
		  inputmethods.cpp \
		  systray.cpp \
		  wait.cpp \
		  shutdownimpl.cpp \
		  launcher.cpp \
		  launcherview.cpp \
		  $$(OPIEDIR)/core/apps/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
   		  irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
		  firstuse.cpp \
		  syncdialog.cpp \
		  serverapp.cpp \
		  qprocess.cpp \
		  qprocess_unix.cpp \
		  screensaver.cpp \
		  $$(OPIEDIR)/noncore/settings/mediummount/mediumwidget.cc \
                  mediadlg.cpp


INCLUDEPATH += $(OPIEDIR)/core/apps/calibrate
DEPENDPATH	+= $(OPIEDIR)/core/apps/calibrate

INCLUDEPATH += $(OPIEDIR)/include $(OPIEDIR)/rsync
DEPENDPATH	+= $(OPIEDIR)/rsync

INCLUDEPATH += $(OPIEDIR)/noncore/settings/mediummount
DEPENDPATH  += $(OPIEDIR)/noncore/settings/mediummount

LIBS        += -lqpe -lopiecore2 -lopieui2 -lopiesecurity2 -lqrsync -lsysfs -lcrypt -lm
TARGET       = qpe

## not ready for use yet
# DEFINES += QPE_HAVE_DIRECT_ACCESS

include( $(OPIEDIR)/include.pro )

contains( CONFIG, LAUNCHER_SYNC_V2 ){
    DEFINES += OPIE_SYNC_V2
    SOURCES += virtualfs.cpp \
               syncaccessmanager.cpp
    HEADERS += virtualfs.h \
               syncaccessmanager.h
    LIBS += -lopiepim2
}

