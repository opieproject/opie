TEMPLATE	= app

CONFIG		= qt warn_on release

DESTDIR		= $(OPIEDIR)/bin

HEADERS		= background.h \
		  desktop.h \
		  qprocess.h \
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
	/skiff/opie/rsync/buf.h \
	/skiff/opie/rsync/checksum.h \
	/skiff/opie/rsync/command.h \
	/skiff/opie/rsync/emit.h \
	/skiff/opie/rsync/job.h \
	/skiff/opie/rsync/netint.h \
	/skiff/opie/rsync/protocol.h \
	/skiff/opie/rsync/prototab.h \
	/skiff/opie/rsync/rsync.h \
	/skiff/opie/rsync/search.h \
	/skiff/opie/rsync/stream.h \
	/skiff/opie/rsync/sumset.h \
	/skiff/opie/rsync/trace.h \
	/skiff/opie/rsync/types.h \
	/skiff/opie/rsync/util.h \
	/skiff/opie/rsync/whole.h \
	/skiff/opie/rsync/config_rsync.h \
	/skiff/opie/rsync/qrsync.h
#		  quicklauncher.h \

SOURCES		= background.cpp \
		  desktop.cpp \
		  qprocess.cpp qprocess_unix.cpp \
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
		  /skiff/opie/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
		irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
	/skiff/opie/rsync/base64.c \
	/skiff/opie/rsync/buf.c \
	/skiff/opie/rsync/checksum.c \
	/skiff/opie/rsync/command.c \
	/skiff/opie/rsync/delta.c \
	/skiff/opie/rsync/emit.c \
	/skiff/opie/rsync/hex.c \
	/skiff/opie/rsync/job.c \
	/skiff/opie/rsync/mdfour.c \
	/skiff/opie/rsync/mksum.c \
	/skiff/opie/rsync/msg.c \
	/skiff/opie/rsync/netint.c \
	/skiff/opie/rsync/patch.c \
	/skiff/opie/rsync/prototab.c \
	/skiff/opie/rsync/readsums.c \
	/skiff/opie/rsync/scoop.c \
	/skiff/opie/rsync/search.c \
	/skiff/opie/rsync/stats.c \
	/skiff/opie/rsync/stream.c \
	/skiff/opie/rsync/sumset.c \
	/skiff/opie/rsync/trace.c \
	/skiff/opie/rsync/tube.c \
	/skiff/opie/rsync/util.c \
	/skiff/opie/rsync/version.c \
	/skiff/opie/rsync/whole.c \
	/skiff/opie/rsync/qrsync.cpp

INTERFACES	= shutdown.ui syncdialog.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include .

INCLUDEPATH += $(OPIEDIR)/calibrate
DEPENDPATH	+= $(OPIEDIR)/calibrate

INCLUDEPATH += $(OPIEDIR)/rsync
DEPENDPATH	+= $(OPIEDIR)/rsync

TARGET		= qpe

LIBS		+= -lqpe -lcrypt

TRANSLATIONS = ../i18n/de/qpe.ts
TRANSLATIONS = ../i18n/pt_BR/qpe.ts
