TEMPLATE	= app

CONFIG		+= qtopia warn_on release
win32:DEFINES   += QTOPIA_DLL

DESTDIR		= $(QPEDIR)/bin

HEADERS		+= server.h \
		  serverinterface.h \
		  launchertab.h \
		  documentlist.h \
		  suspendmonitor.h \
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
		  $(QPEDIR)/src/settings/calibrate/calibrate.h \
		  startmenu.h \
		  transferserver.h \
		  qcopbridge.h \
		  packageslave.h \
		  irserver.h \
		  firstuse.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/buf.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/command.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/emit.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/prototab.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/search.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/types.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/whole.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
	$(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.h \
		  syncdialog.h \
		  serverapp.h

SOURCES		+= server.cpp \
		  serverinterface.cpp \
		  launchertab.cpp \
		  documentlist.cpp \
		  suspendmonitor.cpp \
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
		  $(QPEDIR)/src/settings/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
   		  irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
		  firstuse.cpp \
	$(QPEDIR)/src/3rdparty/libraries/rsync/base64.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/buf.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/command.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/delta.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/emit.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/hex.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/job.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/mdfour.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/mksum.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/msg.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/netint.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/patch.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/prototab.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/readsums.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/scoop.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/search.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/stats.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/stream.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/trace.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/tube.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/util.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/version.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/whole.c \
	$(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.cpp \
		  syncdialog.cpp \
		  serverapp.cpp

INTERFACES	+= shutdown.ui

INCLUDEPATH += $(QPEDIR)/src/settings/calibrate
DEPENDPATH	+= $(QPEDIR)/src/settings/calibrate

INCLUDEPATH += $(QPEDIR)/src/3rdparty/libraries/rsync
DEPENDPATH	+= $(QPEDIR)/src/3rdparty/libraries/rsync

TARGET		= qpe

unix:LIBS	+= -lcrypt -luuid
win32:LIBS      += 

TRANSLATIONS = qpe-en_GB.ts qpe-de.ts qpe-ja.ts qpe-no.ts
