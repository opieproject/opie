TEMPLATE	= app

CONFIG		+= qt warn_on 

DESTDIR		= $$(OPIEDIR)/bin

HEADERS		+= server.h \
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

LIBS        += -lqpe -lopiecore2 -lopieui2 -lqrsync
TARGET       = opie

contains( $(CONFIG_TARGET_MACOSX), y ) {
  LIBS += -lcrypt
}

include ( $(OPIEDIR)/include.pro )
