TEMPLATE	= app

CONFIG		= qt warn_on release

DESTDIR		= $(QPEDIR)/bin

HEADERS		= background.h \
		  desktop.h \
		  info.h \
		  appicons.h \
		  taskbar.h \
		  sidething.h \
		  mrulist.h \
		  stabmon.h \
		  inputmethods.h \
		  systray.h \
		  wait.h \
		  shutdownimpl.h \
		  launcher.h \
		  launcherview.h \
		  ../calibrate/calibrate.h \
		  startmenu.h \
		  transferserver.h \
		  qcopbridge.h \
		  packageslave.h \
		irserver.h \
	$(QPEDIR)/rsync/buf.h \
	$(QPEDIR)/rsync/checksum.h \
	$(QPEDIR)/rsync/command.h \
	$(QPEDIR)/rsync/emit.h \
	$(QPEDIR)/rsync/job.h \
	$(QPEDIR)/rsync/netint.h \
	$(QPEDIR)/rsync/protocol.h \
	$(QPEDIR)/rsync/prototab.h \
	$(QPEDIR)/rsync/rsync.h \
	$(QPEDIR)/rsync/search.h \
	$(QPEDIR)/rsync/stream.h \
	$(QPEDIR)/rsync/sumset.h \
	$(QPEDIR)/rsync/trace.h \
	$(QPEDIR)/rsync/types.h \
	$(QPEDIR)/rsync/util.h \
	$(QPEDIR)/rsync/whole.h \
	$(QPEDIR)/rsync/config_rsync.h \
	$(QPEDIR)/rsync/qrsync.h
#		  quicklauncher.h \

SOURCES		= background.cpp \
		  desktop.cpp \
		  info.cpp \
		  appicons.cpp \
		  taskbar.cpp \
		  sidething.cpp \
		  mrulist.cpp \
		  stabmon.cpp \
		  inputmethods.cpp \
		  systray.cpp \
		  wait.cpp \
		  shutdownimpl.cpp \
		  launcher.cpp \
		  launcherview.cpp \
		  $(QPEDIR)/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
		irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
	$(QPEDIR)/rsync/base64.c \
	$(QPEDIR)/rsync/buf.c \
	$(QPEDIR)/rsync/checksum.c \
	$(QPEDIR)/rsync/command.c \
	$(QPEDIR)/rsync/delta.c \
	$(QPEDIR)/rsync/emit.c \
	$(QPEDIR)/rsync/hex.c \
	$(QPEDIR)/rsync/job.c \
	$(QPEDIR)/rsync/mdfour.c \
	$(QPEDIR)/rsync/mksum.c \
	$(QPEDIR)/rsync/msg.c \
	$(QPEDIR)/rsync/netint.c \
	$(QPEDIR)/rsync/patch.c \
	$(QPEDIR)/rsync/prototab.c \
	$(QPEDIR)/rsync/readsums.c \
	$(QPEDIR)/rsync/scoop.c \
	$(QPEDIR)/rsync/search.c \
	$(QPEDIR)/rsync/stats.c \
	$(QPEDIR)/rsync/stream.c \
	$(QPEDIR)/rsync/sumset.c \
	$(QPEDIR)/rsync/trace.c \
	$(QPEDIR)/rsync/tube.c \
	$(QPEDIR)/rsync/util.c \
	$(QPEDIR)/rsync/version.c \
	$(QPEDIR)/rsync/whole.c \
	$(QPEDIR)/rsync/qrsync.cpp

INTERFACES	= shutdown.ui syncdialog.ui

INCLUDEPATH += $(QPEDIR)/include
DEPENDPATH	+= $(QPEDIR)/include .

INCLUDEPATH += $(QPEDIR)/calibrate
DEPENDPATH	+= $(QPEDIR)/calibrate

INCLUDEPATH += $(QPEDIR)/rsync
DEPENDPATH	+= $(QPEDIR)/rsync

TARGET		= qpe

LIBS		+= -lqpe -lcrypt

TRANSLATIONS = ../i18n/de/qpe.ts
