TEMPLATE	= app

CONFIG		= qt warn_on release

DESTDIR		= $(OPIEDIR)/bin

HEADERS		= background.h \
		  desktop.h \
		  qprocess.h \
		  mediummountgui.h \
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
		  $(OPIEDIR)/core/apps/calibrate/calibrate.h \
		  startmenu.h \
		  transferserver.h \
		  qcopbridge.h \
		  packageslave.h \
		irserver.h \
	$(OPIEDIR)/rsync/buf.h \
	$(OPIEDIR)/rsync/checksum.h \
	$(OPIEDIR)/rsync/command.h \
	$(OPIEDIR)/rsync/emit.h \
	$(OPIEDIR)/rsync/job.h \
	$(OPIEDIR)/rsync/netint.h \
	$(OPIEDIR)/rsync/protocol.h \
	$(OPIEDIR)/rsync/prototab.h \
	$(OPIEDIR)/rsync/rsync.h \
	$(OPIEDIR)/rsync/search.h \
	$(OPIEDIR)/rsync/stream.h \
	$(OPIEDIR)/rsync/sumset.h \
	$(OPIEDIR)/rsync/trace.h \
	$(OPIEDIR)/rsync/types.h \
	$(OPIEDIR)/rsync/util.h \
	$(OPIEDIR)/rsync/whole.h \
	$(OPIEDIR)/rsync/config_rsync.h \
	$(OPIEDIR)/rsync/qrsync.h
#		  quicklauncher.h \

SOURCES		= background.cpp \
		  desktop.cpp \
		  mediummountgui.cpp \
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
		  $(OPIEDIR)/core/apps/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
		irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
	$(OPIEDIR)/rsync/base64.c \
	$(OPIEDIR)/rsync/buf.c \
	$(OPIEDIR)/rsync/checksum.c \
	$(OPIEDIR)/rsync/command.c \
	$(OPIEDIR)/rsync/delta.c \
	$(OPIEDIR)/rsync/emit.c \
	$(OPIEDIR)/rsync/hex.c \
	$(OPIEDIR)/rsync/job.c \
	$(OPIEDIR)/rsync/mdfour.c \
	$(OPIEDIR)/rsync/mksum.c \
	$(OPIEDIR)/rsync/msg.c \
	$(OPIEDIR)/rsync/netint.c \
	$(OPIEDIR)/rsync/patch.c \
	$(OPIEDIR)/rsync/prototab.c \
	$(OPIEDIR)/rsync/readsums.c \
	$(OPIEDIR)/rsync/scoop.c \
	$(OPIEDIR)/rsync/search.c \
	$(OPIEDIR)/rsync/stats.c \
	$(OPIEDIR)/rsync/stream.c \
	$(OPIEDIR)/rsync/sumset.c \
	$(OPIEDIR)/rsync/trace.c \
	$(OPIEDIR)/rsync/tube.c \
	$(OPIEDIR)/rsync/util.c \
	$(OPIEDIR)/rsync/version.c \
	$(OPIEDIR)/rsync/whole.c \
	$(OPIEDIR)/rsync/qrsync.cpp

INTERFACES	= shutdown.ui syncdialog.ui

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include .

INCLUDEPATH += $(OPIEDIR)/core/apps/calibrate
DEPENDPATH	+= $(OPIEDIR)/core/apps/calibrate

INCLUDEPATH += $(OPIEDIR)/rsync
DEPENDPATH	+= $(OPIEDIR)/rsync

TARGET		= opie

LIBS		+= -lqpe -lcrypt -lopie

TRANSLATIONS = ../i18n/de/qpe.ts
TRANSLATIONS += ../i18n/es/qpe.ts
TRANSLATIONS += ../i18n/pt/qpe.ts
TRANSLATIONS += ../i18n/pt_BR/qpe.ts
TRANSLATIONS   += ../i18n/en/qpe.ts
TRANSLATIONS   += ../i18n/hu/qpe.ts
TRANSLATIONS   += ../i18n/ja/qpe.ts
TRANSLATIONS   += ../i18n/fr/qpe.ts
TRANSLATIONS   += ../i18n/ko/qpe.ts
TRANSLATIONS   += ../i18n/no/qpe.ts
TRANSLATIONS   += ../i18n/zh_CN/qpe.ts
TRANSLATIONS   += ../i18n/zh_TW/qpe.ts
