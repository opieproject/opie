TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= ../../bin
HEADERS		= background.h \
		  desktop.h \
		  screensaver.h \
		  qprocess.h \
		  mediummountgui.h \
		  info.h \
		  appicons.h \
		  taskbar.h \
		  sidething.h \
		  runningappbar.h \
		  stabmon.h \
		  inputmethods.h \
		  systray.h \
		  wait.h \
		  shutdownimpl.h \
		  launcher.h \
		  launcherview.h \
		  ../../core/apps/calibrate/calibrate.h \
		  startmenu.h \
		  transferserver.h \
		  qcopbridge.h \
		  packageslave.h \
		irserver.h \
	../../rsync/buf.h \
	../../rsync/checksum.h \
	../../rsync/command.h \
	../../rsync/emit.h \
	../../rsync/job.h \
	../../rsync/netint.h \
	../../rsync/protocol.h \
	../../rsync/prototab.h \
	../../rsync/rsync.h \
	../../rsync/search.h \
	../../rsync/stream.h \
	../../rsync/sumset.h \
	../../rsync/trace.h \
	../../rsync/types.h \
	../../rsync/util.h \
	../../rsync/whole.h \
	../../rsync/config_rsync.h \
	../../rsync/qrsync.h \
		  quicklauncher.h 
SOURCES		= background.cpp \
		  desktop.cpp \
		  screensaver.cpp \
		  mediummountgui.cpp \
		  qprocess.cpp qprocess_unix.cpp \
		  info.cpp \
		  appicons.cpp \
		  taskbar.cpp \
		  sidething.cpp \
 	          runningappbar.cpp \
		  stabmon.cpp \
		  inputmethods.cpp \
		  systray.cpp \
		  wait.cpp \
		  shutdownimpl.cpp \
		  launcher.cpp \
		  launcherview.cpp \
		  ../../core/apps/calibrate/calibrate.cpp \
		  transferserver.cpp \
		  packageslave.cpp \
		irserver.cpp \
		  qcopbridge.cpp \
		  startmenu.cpp \
		  main.cpp \
	../../rsync/base64.c \
	../../rsync/buf.c \
	../../rsync/checksum.c \
	../../rsync/command.c \
	../../rsync/delta.c \
	../../rsync/emit.c \
	../../rsync/hex.c \
	../../rsync/job.c \
	../../rsync/mdfour.c \
	../../rsync/mksum.c \
	../../rsync/msg.c \
	../../rsync/netint.c \
	../../rsync/patch.c \
	../../rsync/prototab.c \
	../../rsync/readsums.c \
	../../rsync/scoop.c \
	../../rsync/search.c \
	../../rsync/stats.c \
	../../rsync/stream.c \
	../../rsync/sumset.c \
	../../rsync/trace.c \
	../../rsync/tube.c \
	../../rsync/util.c \
	../../rsync/version.c \
	../../rsync/whole.c \
	../../rsync/qrsync.cpp
INTERFACES	= syncdialog.ui
INCLUDEPATH += ../../include
DEPENDPATH	+= ../../include .
INCLUDEPATH += ../../core/apps/calibrate
DEPENDPATH	+= ../../core/apps/calibrate
INCLUDEPATH += ../../rsync
DEPENDPATH	+= ../../rsync
TARGET		= qpe
LIBS		+= -lqpe -lcrypt -lopie 

TRANSLATIONS = ../../i18n/de/qpe.ts \
	 ../../i18n/xx/qpe.ts \
	 ../../i18n/en/qpe.ts \
	 ../../i18n/es/qpe.ts \
	 ../../i18n/fr/qpe.ts \
	 ../../i18n/hu/qpe.ts \
	 ../../i18n/ja/qpe.ts \
	 ../../i18n/ko/qpe.ts \
	 ../../i18n/no/qpe.ts \
	 ../../i18n/pl/qpe.ts \
	 ../../i18n/pt/qpe.ts \
	 ../../i18n/pt_BR/qpe.ts \
	 ../../i18n/sl/qpe.ts \
	 ../../i18n/zh_CN/qpe.ts \
	 ../../i18n/it/qpe.ts \
	 ../../i18n/zh_TW/qpe.ts \
	 ../../i18n/da/qpe.ts
