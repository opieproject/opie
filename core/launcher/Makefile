#############################################################################
# Automatically generated from server/Makefile.in
# Build options from 
#############################################################################

default: all  # package

CONTROL = server/qpe-taskbar.control

# Compiling
INTERFACE_DECL_PATH 	= .
SYSCONF_CXX		= g++
SYSCONF_CC		= gcc
DASHCROSS		= 

# Compiling with support libraries
SYSCONF_CXXFLAGS_X11	= 
SYSCONF_CXXFLAGS_QT	= -I$(QTDIR)/include
SYSCONF_CXXFLAGS_QTOPIA	= -I$(QPEDIR)/include
SYSCONF_CFLAGS_QTOPIA	= -I$(QPEDIR)/include
SYSCONF_CXXFLAGS_OPENGL	= -I/usr/X11R6/include

# Compiling YACC output
SYSCONF_CXXFLAGS_YACC     = -Wno-unused -Wno-parentheses

# Linking with support libraries
SYSCONF_RPATH_X11	= 
SYSCONF_RPATH_QT	= -Wl,-rpath,$(QTDIR)/lib
SYSCONF_RPATH_QTOPIA	= -Wl,-rpath,$(QPEDIR)/lib
SYSCONF_RPATH_OPENGL	= -Wl,-rpath,/usr/X11R6/lib

# Linking with support libraries
# X11
SYSCONF_LFLAGS_X11	= 
SYSCONF_LIBS_X11	= 
# Qt, Qt+OpenGL
SYSCONF_LFLAGS_QT	= -L$(QTDIR)/lib
SYSCONF_LFLAGS_QTOPIA	= -L$(QPEDIR)/lib
SYSCONF_LIBS_QT		= -lqte$(QT_THREAD_SUFFIX)
SYSCONF_LIBS_QT_OPENGL	= 
SYSCONF_LIBS_QTOPIA	= -lqtopia
# OpenGL
SYSCONF_LFLAGS_OPENGL	= -L/usr/X11R6/lib
SYSCONF_LIBS_OPENGL	= 
# Yacc
SYSCONF_LIBS_YACC	= 

# Linking applications
SYSCONF_LINK		= gcc
SYSCONF_LFLAGS		= 
SYSCONF_LIBS		= 

# Link flags for shared objects
SYSCONF_LFLAGS_SHOBJ	= -shared

# Flags for threading
SYSCONF_CFLAGS_THREAD	= -D_REENTRANT
SYSCONF_CXXFLAGS_THREAD	= -D_REENTRANT
SYSCONF_LFLAGS_THREAD	= 
SYSCONF_LIBS_THREAD	=  -lpthread

# Meta-object compiler
SYSCONF_MOC		= $(QTDIR)/bin/moc

# UI compiler
SYSCONF_UIC		= $(QTDIR)/bin/uic

# Linking shared libraries
#   - Build the $(TARGET) library, eg. lib$(TARGET).so.2.2.2
#   - Place target in $(DESTDIR) - which has a trailing /
#   - Usually needs to incorporate $(VER_MAJ), $(VER_MIN) and $(VER_PATCH)
#
SYSCONF_LINK_SHLIB	= gcc
SYSCONF_LINK_TARGET_SHARED	= lib$(TARGET).so.$(VER_MAJ).$(VER_MIN).$(VER_PATCH)
SYSCONF_LINK_LIB_SHARED	=  $(SYSCONF_LINK_SHLIB) -shared -Wl,-soname,lib$(TARGET).so.$(VER_MAJ) \
				     $(LFLAGS) -o $(SYSCONF_LINK_TARGET_SHARED) \
				     $(OBJECTS) $(OBJMOC) $(LIBS) && \
				 mv $(SYSCONF_LINK_TARGET_SHARED) $(DESTDIR); \
				 cd $(DESTDIR) && \
				 rm -f lib$(TARGET).so lib$(TARGET).so.$(VER_MAJ) lib$(TARGET).so.$(VER_MAJ).$(VER_MIN); \
				 ln -s $(SYSCONF_LINK_TARGET_SHARED) lib$(TARGET).so; \
				 ln -s $(SYSCONF_LINK_TARGET_SHARED) lib$(TARGET).so.$(VER_MAJ); \
				 ln -s $(SYSCONF_LINK_TARGET_SHARED) lib$(TARGET).so.$(VER_MAJ).$(VER_MIN)

# Linking static libraries
#   - Build the $(TARGET) library, eg. lib$(TARGET).a
#   - Place target in $(DESTDIR) - which has a trailing /
#
SYSCONF_AR		= ar cqs
SYSCONF_LINK_TARGET_STATIC = lib$(TARGET).a
SYSCONF_LINK_LIB_STATIC	= rm -f $(DESTDIR)$(SYSCONF_LINK_TARGET_STATIC) ; \
				 $(SYSCONF_AR) $(DESTDIR)$(SYSCONF_LINK_TARGET_STATIC) $(OBJECTS) $(OBJMOC) 
# Compiling application source
SYSCONF_CXXFLAGS	= -pipe -DQWS -fno-exceptions -fno-rtti -g -Wall -W
SYSCONF_CFLAGS		= -pipe -g -Wall -W
# Default link type (static linking is still be used where required)
SYSCONF_LINK_LIB	= $(SYSCONF_LINK_LIB_SHARED)
SYSCONF_LINK_TARGET	= $(SYSCONF_LINK_TARGET_SHARED)
# Compiling library source
SYSCONF_CXXFLAGS_LIB	= -fPIC
SYSCONF_CFLAGS_LIB	= -fPIC
# Compiling shared-object source
SYSCONF_CXXFLAGS_SHOBJ	= -fPIC
SYSCONF_CFLAGS_SHOBJ	= -fPIC
# Linking Qt
SYSCONF_LIBS_QTLIB	= $(SYSCONF_LFLAGS_X11) $(QT_LIBS_MT) $(QT_LIBS_OPT)
# Linking Qt applications
SYSCONF_LIBS_QTAPP	= 

# Platform output directories
MOC_DIR = .moc/linux-generic-g++/
OBJECTS_DIR = .obj/linux-generic-g++/

#############################################################################

####### Compiler, tools and options

CXX	=	$(SYSCONF_CXX) $(QT_CXX_MT)
CXXFLAGS=	$(SYSCONF_CXXFLAGS_QTOPIA) $(SYSCONF_CXXFLAGS_QT) $(SYSCONF_CXXFLAGS) -DQTOPIA_APP_INTERFACE
CC	=	$(SYSCONF_CC) $(QT_C_MT)
CFLAGS	=	$(SYSCONF_CFLAGS_QTOPIA) $(SYSCONF_CFLAGS) -DQTOPIA_APP_INTERFACE
INCPATH =	-I$(MOC_DIR) -I$(QPEDIR)/src/settings/calibrate -I$(QPEDIR)/src/3rdparty/libraries/rsync
LFLAGS	=	$(SYSCONF_LFLAGS_QTOPIA) $(SYSCONF_RPATH_QTOPIA) $(SYSCONF_LFLAGS_QT) $(SYSCONF_RPATH_QT) $(SYSCONF_LFLAGS) $(QT_LFLAGS_MT)
LIBS	=	$(SUBLIBS) -ljpeg -Wl,-Bstatic -Wl,-whole-archive  -Wl,-no-whole-archive -Wl,-Bdynamic -lcrypt -luuid -lqpe $(SYSCONF_LIBS_QTOPIA) $(SYSCONF_LIBS_QT) $(SYSCONF_LIBS) $(SYSCONF_LIBS_QTAPP)
MOC	=	$(SYSCONF_MOC)
UIC	=	$(SYSCONF_UIC)

####### Target

DESTDIR = $(QPEDIR)/bin/
VER_MAJ = 1
VER_MIN = 0
VER_PATCH = 0
TARGET	= qpe
TARGET1 = lib$(TARGET).so.$(VER_MAJ)

####### Files

HEADERS =	server.h \
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
SOURCES =	server.cpp \
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
OBJECTS =	$(OBJECTS_DIR)/server.o \
		$(OBJECTS_DIR)/serverinterface.o \
		$(OBJECTS_DIR)/launchertab.o \
		$(OBJECTS_DIR)/documentlist.o \
		$(OBJECTS_DIR)/suspendmonitor.o \
		$(OBJECTS_DIR)/appicons.o \
		$(OBJECTS_DIR)/taskbar.o \
		$(OBJECTS_DIR)/runningappbar.o \
		$(OBJECTS_DIR)/applauncher.o \
		$(OBJECTS_DIR)/stabmon.o \
		$(OBJECTS_DIR)/inputmethods.o \
		$(OBJECTS_DIR)/systray.o \
		$(OBJECTS_DIR)/wait.o \
		$(OBJECTS_DIR)/shutdownimpl.o \
		$(OBJECTS_DIR)/launcher.o \
		$(OBJECTS_DIR)/launcherview.o \
		$(OBJECTS_DIR)/calibrate.o \
		$(OBJECTS_DIR)/transferserver.o \
		$(OBJECTS_DIR)/packageslave.o \
		$(OBJECTS_DIR)/irserver.o \
		$(OBJECTS_DIR)/qcopbridge.o \
		$(OBJECTS_DIR)/startmenu.o \
		$(OBJECTS_DIR)/main.o \
		$(OBJECTS_DIR)/firstuse.o \
		$(OBJECTS_DIR)/base64.o \
		$(OBJECTS_DIR)/buf.o \
		$(OBJECTS_DIR)/checksum.o \
		$(OBJECTS_DIR)/command.o \
		$(OBJECTS_DIR)/delta.o \
		$(OBJECTS_DIR)/emit.o \
		$(OBJECTS_DIR)/hex.o \
		$(OBJECTS_DIR)/job.o \
		$(OBJECTS_DIR)/mdfour.o \
		$(OBJECTS_DIR)/mksum.o \
		$(OBJECTS_DIR)/msg.o \
		$(OBJECTS_DIR)/netint.o \
		$(OBJECTS_DIR)/patch.o \
		$(OBJECTS_DIR)/prototab.o \
		$(OBJECTS_DIR)/readsums.o \
		$(OBJECTS_DIR)/scoop.o \
		$(OBJECTS_DIR)/search.o \
		$(OBJECTS_DIR)/stats.o \
		$(OBJECTS_DIR)/stream.o \
		$(OBJECTS_DIR)/sumset.o \
		$(OBJECTS_DIR)/trace.o \
		$(OBJECTS_DIR)/tube.o \
		$(OBJECTS_DIR)/util.o \
		$(OBJECTS_DIR)/version.o \
		$(OBJECTS_DIR)/whole.o \
		$(OBJECTS_DIR)/qrsync.o \
		$(OBJECTS_DIR)/syncdialog.o \
		$(OBJECTS_DIR)/serverapp.o \
		$(OBJECTS_DIR)/shutdown.o
INTERFACES =    shutdown.ui
UICDECLS =      shutdown.h
UICIMPLS =      shutdown.cpp
SRCMOC	=	$(MOC_DIR)/moc_server.cpp \
		$(MOC_DIR)/moc_launchertab.cpp \
		$(MOC_DIR)/moc_documentlist.cpp \
		$(MOC_DIR)/moc_suspendmonitor.cpp \
		$(MOC_DIR)/moc_appicons.cpp \
		$(MOC_DIR)/moc_taskbar.cpp \
		$(MOC_DIR)/moc_runningappbar.cpp \
		$(MOC_DIR)/moc_applauncher.cpp \
		$(MOC_DIR)/moc_inputmethods.cpp \
		$(MOC_DIR)/moc_systray.cpp \
		$(MOC_DIR)/moc_shutdownimpl.cpp \
		$(MOC_DIR)/moc_launcher.cpp \
		$(MOC_DIR)/moc_launcherview.cpp \
		$(MOC_DIR)/moc_calibrate.cpp \
		$(MOC_DIR)/moc_startmenu.cpp \
		$(MOC_DIR)/moc_transferserver.cpp \
		$(MOC_DIR)/moc_qcopbridge.cpp \
		$(MOC_DIR)/moc_packageslave.cpp \
		$(MOC_DIR)/moc_irserver.cpp \
		$(MOC_DIR)/moc_firstuse.cpp \
		$(MOC_DIR)/moc_syncdialog.cpp \
		$(MOC_DIR)/moc_serverapp.cpp \
		$(MOC_DIR)/moc_shutdown.cpp \
		$(MOC_DIR)/serverinterface.moc \
		$(MOC_DIR)/documentlist.moc \
		$(MOC_DIR)/appicons.moc \
		$(MOC_DIR)/taskbar.moc \
		$(MOC_DIR)/serverapp.moc
OBJMOC	=	$(OBJECTS_DIR)/moc_server.o \
		$(OBJECTS_DIR)/moc_launchertab.o \
		$(OBJECTS_DIR)/moc_documentlist.o \
		$(OBJECTS_DIR)/moc_suspendmonitor.o \
		$(OBJECTS_DIR)/moc_appicons.o \
		$(OBJECTS_DIR)/moc_taskbar.o \
		$(OBJECTS_DIR)/moc_runningappbar.o \
		$(OBJECTS_DIR)/moc_applauncher.o \
		$(OBJECTS_DIR)/moc_inputmethods.o \
		$(OBJECTS_DIR)/moc_systray.o \
		$(OBJECTS_DIR)/moc_shutdownimpl.o \
		$(OBJECTS_DIR)/moc_launcher.o \
		$(OBJECTS_DIR)/moc_launcherview.o \
		$(OBJECTS_DIR)/moc_calibrate.o \
		$(OBJECTS_DIR)/moc_startmenu.o \
		$(OBJECTS_DIR)/moc_transferserver.o \
		$(OBJECTS_DIR)/moc_qcopbridge.o \
		$(OBJECTS_DIR)/moc_packageslave.o \
		$(OBJECTS_DIR)/moc_irserver.o \
		$(OBJECTS_DIR)/moc_firstuse.o \
		$(OBJECTS_DIR)/moc_syncdialog.o \
		$(OBJECTS_DIR)/moc_serverapp.o \
		$(OBJECTS_DIR)/moc_shutdown.o


####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) $(SUBLIBS)
	$(SYSCONF_LINK) $(LFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

tmake:
	tmake server.pro

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS)
	-rm -f *~ core
	-rm -f allmoc.cpp

####### Extension Modules

listpromodules:
	@echo 

listallmodules:
	@echo 

listaddonpromodules:
	@echo 

listaddonentmodules:
	@echo 


REQUIRES=

####### Sub-libraries


###### Combined headers



####### Compile

$(OBJECTS_DIR)/server.o: server.cpp \
		server.h \
		serverapp.h \
		shutdownimpl.h \
		shutdown.h \
		launcher.h \
		launcherview.h \
		launchertab.h \
		serverinterface.h \
		startmenu.h \
		transferserver.h \
		qcopbridge.h \
		irserver.h \
		packageslave.h \
		$(QPEDIR)/src/settings/calibrate/calibrate.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.h \
		syncdialog.h \
		applauncher.h \
		suspendmonitor.h \
		documentlist.h \
		apps.h \
		../applications/addressbook/addressbook.h \
		../applications/datebook/datebook.h \
		../applications/todo/mainwindow.h \
		../libraries/qtopiacalc/calculator.h \
		../applications/clock/clock.h \
		../applications/helpbrowser/helpbrowser.h \
		../applications/imageviewer/showimg.h \
		../applications/mediaplayer/maindocumentwidgetstack.h \
		../applications/textedit/textedit.h \
		../games/fifteen/fifteen.h \
		../games/mindbreaker/mindbreaker.h \
		../games/minesweep/minesweep.h \
		../games/qasteroids/toplevel.h \
		../games/snake/interface.h \
		../games/solitaire/canvascardwindow.h \
		../settings/systemtime/settime.h \
		../settings/appearance/appearance.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/server.o server.cpp

$(OBJECTS_DIR)/serverinterface.o: serverinterface.cpp \
		$(MOC_DIR)/serverinterface.moc \
		serverinterface.h \
		server.h \
		documentlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/serverinterface.o serverinterface.cpp

$(OBJECTS_DIR)/launchertab.o: launchertab.cpp \
		launchertab.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/launchertab.o launchertab.cpp

$(OBJECTS_DIR)/documentlist.o: documentlist.cpp \
		$(MOC_DIR)/documentlist.moc \
		documentlist.h \
		serverinterface.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/documentlist.o documentlist.cpp

$(OBJECTS_DIR)/suspendmonitor.o: suspendmonitor.cpp \
		suspendmonitor.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/suspendmonitor.o suspendmonitor.cpp

$(OBJECTS_DIR)/appicons.o: appicons.cpp \
		$(MOC_DIR)/appicons.moc \
		appicons.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/appicons.o appicons.cpp

$(OBJECTS_DIR)/taskbar.o: taskbar.cpp \
		$(MOC_DIR)/taskbar.moc \
		startmenu.h \
		inputmethods.h \
		runningappbar.h \
		systray.h \
		wait.h \
		appicons.h \
		taskbar.h \
		serverinterface.h \
		server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/taskbar.o taskbar.cpp

$(OBJECTS_DIR)/runningappbar.o: runningappbar.cpp \
		runningappbar.h \
		serverinterface.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/runningappbar.o runningappbar.cpp

$(OBJECTS_DIR)/applauncher.o: applauncher.cpp \
		applauncher.h \
		documentlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/applauncher.o applauncher.cpp

$(OBJECTS_DIR)/stabmon.o: stabmon.cpp \
		stabmon.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/stabmon.o stabmon.cpp

$(OBJECTS_DIR)/inputmethods.o: inputmethods.cpp \
		inputmethods.h \
		../plugins/inputmethods/handwriting/handwritingimpl.h \
		../plugins/inputmethods/keyboard/keyboardimpl.h \
		../3rdparty/plugins/inputmethods/pickboard/pickboardimpl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/inputmethods.o inputmethods.cpp

$(OBJECTS_DIR)/systray.o: systray.cpp \
		systray.h \
		../plugins/applets/clockapplet/clockappletimpl.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/systray.o systray.cpp

$(OBJECTS_DIR)/wait.o: wait.cpp \
		wait.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/wait.o wait.cpp

$(OBJECTS_DIR)/shutdownimpl.o: shutdownimpl.cpp \
		shutdownimpl.h \
		shutdown.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/shutdownimpl.o shutdownimpl.cpp

$(OBJECTS_DIR)/launcher.o: launcher.cpp \
		startmenu.h \
		taskbar.h \
		serverinterface.h \
		launcherview.h \
		launcher.h \
		launchertab.h \
		server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/launcher.o launcher.cpp

$(OBJECTS_DIR)/launcherview.o: launcherview.cpp \
		launcherview.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/launcherview.o launcherview.cpp

$(OBJECTS_DIR)/calibrate.o: $(QPEDIR)/src/settings/calibrate/calibrate.cpp \
		$(QPEDIR)/src/settings/calibrate/calibrate.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/calibrate.o $(QPEDIR)/src/settings/calibrate/calibrate.cpp

$(OBJECTS_DIR)/transferserver.o: transferserver.cpp \
		transferserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/transferserver.o transferserver.cpp

$(OBJECTS_DIR)/packageslave.o: packageslave.cpp \
		packageslave.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/packageslave.o packageslave.cpp

$(OBJECTS_DIR)/irserver.o: irserver.cpp \
		irserver.h \
		obexinterface.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/irserver.o irserver.cpp

$(OBJECTS_DIR)/qcopbridge.o: qcopbridge.cpp \
		qcopbridge.h \
		transferserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/qcopbridge.o qcopbridge.cpp

$(OBJECTS_DIR)/startmenu.o: startmenu.cpp \
		startmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/startmenu.o startmenu.cpp

$(OBJECTS_DIR)/main.o: main.cpp \
		server.h \
		serverapp.h \
		shutdownimpl.h \
		shutdown.h \
		taskbar.h \
		serverinterface.h \
		startmenu.h \
		stabmon.h \
		launcher.h \
		launcherview.h \
		launchertab.h \
		firstuse.h \
		$(QPEDIR)/src/settings/calibrate/../calibrate/calibrate.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/main.o main.cpp

$(OBJECTS_DIR)/firstuse.o: firstuse.cpp \
		firstuse.h \
		inputmethods.h \
		applauncher.h \
		serverapp.h \
		shutdownimpl.h \
		shutdown.h \
		../settings/calibrate/calibrate.h \
		documentlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/firstuse.o firstuse.cpp

$(OBJECTS_DIR)/base64.o: $(QPEDIR)/src/3rdparty/libraries/rsync/base64.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/base64.o $(QPEDIR)/src/3rdparty/libraries/rsync/base64.c

$(OBJECTS_DIR)/buf.o: $(QPEDIR)/src/3rdparty/libraries/rsync/buf.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/buf.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/buf.o $(QPEDIR)/src/3rdparty/libraries/rsync/buf.c

$(OBJECTS_DIR)/checksum.o: $(QPEDIR)/src/3rdparty/libraries/rsync/checksum.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/checksum.o $(QPEDIR)/src/3rdparty/libraries/rsync/checksum.c

$(OBJECTS_DIR)/command.o: $(QPEDIR)/src/3rdparty/libraries/rsync/command.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/command.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/command.o $(QPEDIR)/src/3rdparty/libraries/rsync/command.c

$(OBJECTS_DIR)/delta.o: $(QPEDIR)/src/3rdparty/libraries/rsync/delta.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/emit.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/search.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/types.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/delta.o $(QPEDIR)/src/3rdparty/libraries/rsync/delta.c

$(OBJECTS_DIR)/emit.o: $(QPEDIR)/src/3rdparty/libraries/rsync/emit.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/command.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/emit.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/prototab.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/emit.o $(QPEDIR)/src/3rdparty/libraries/rsync/emit.c

$(OBJECTS_DIR)/hex.o: $(QPEDIR)/src/3rdparty/libraries/rsync/hex.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/hex.o $(QPEDIR)/src/3rdparty/libraries/rsync/hex.c

$(OBJECTS_DIR)/job.o: $(QPEDIR)/src/3rdparty/libraries/rsync/job.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/job.o $(QPEDIR)/src/3rdparty/libraries/rsync/job.c

$(OBJECTS_DIR)/mdfour.o: $(QPEDIR)/src/3rdparty/libraries/rsync/mdfour.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/types.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/mdfour.o $(QPEDIR)/src/3rdparty/libraries/rsync/mdfour.c

$(OBJECTS_DIR)/mksum.o: $(QPEDIR)/src/3rdparty/libraries/rsync/mksum.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/mksum.o $(QPEDIR)/src/3rdparty/libraries/rsync/mksum.c

$(OBJECTS_DIR)/msg.o: $(QPEDIR)/src/3rdparty/libraries/rsync/msg.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/msg.o $(QPEDIR)/src/3rdparty/libraries/rsync/msg.c

$(OBJECTS_DIR)/netint.o: $(QPEDIR)/src/3rdparty/libraries/rsync/netint.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/netint.o $(QPEDIR)/src/3rdparty/libraries/rsync/netint.c

$(OBJECTS_DIR)/patch.o: $(QPEDIR)/src/3rdparty/libraries/rsync/patch.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/command.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/prototab.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/patch.o $(QPEDIR)/src/3rdparty/libraries/rsync/patch.c

$(OBJECTS_DIR)/prototab.o: $(QPEDIR)/src/3rdparty/libraries/rsync/prototab.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/command.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/prototab.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/prototab.o $(QPEDIR)/src/3rdparty/libraries/rsync/prototab.c

$(OBJECTS_DIR)/readsums.o: $(QPEDIR)/src/3rdparty/libraries/rsync/readsums.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/netint.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/protocol.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/readsums.o $(QPEDIR)/src/3rdparty/libraries/rsync/readsums.c

$(OBJECTS_DIR)/scoop.o: $(QPEDIR)/src/3rdparty/libraries/rsync/scoop.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/scoop.o $(QPEDIR)/src/3rdparty/libraries/rsync/scoop.c

$(OBJECTS_DIR)/search.o: $(QPEDIR)/src/3rdparty/libraries/rsync/search.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/search.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/checksum.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/search.o $(QPEDIR)/src/3rdparty/libraries/rsync/search.c

$(OBJECTS_DIR)/stats.o: $(QPEDIR)/src/3rdparty/libraries/rsync/stats.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/stats.o $(QPEDIR)/src/3rdparty/libraries/rsync/stats.c

$(OBJECTS_DIR)/stream.o: $(QPEDIR)/src/3rdparty/libraries/rsync/stream.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/stream.o $(QPEDIR)/src/3rdparty/libraries/rsync/stream.c

$(OBJECTS_DIR)/sumset.o: $(QPEDIR)/src/3rdparty/libraries/rsync/sumset.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/sumset.o $(QPEDIR)/src/3rdparty/libraries/rsync/sumset.c

$(OBJECTS_DIR)/trace.o: $(QPEDIR)/src/3rdparty/libraries/rsync/trace.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/trace.o $(QPEDIR)/src/3rdparty/libraries/rsync/trace.c

$(OBJECTS_DIR)/tube.o: $(QPEDIR)/src/3rdparty/libraries/rsync/tube.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/stream.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/tube.o $(QPEDIR)/src/3rdparty/libraries/rsync/tube.c

$(OBJECTS_DIR)/util.o: $(QPEDIR)/src/3rdparty/libraries/rsync/util.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/util.o $(QPEDIR)/src/3rdparty/libraries/rsync/util.c

$(OBJECTS_DIR)/version.o: $(QPEDIR)/src/3rdparty/libraries/rsync/version.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/version.o $(QPEDIR)/src/3rdparty/libraries/rsync/version.c

$(OBJECTS_DIR)/whole.o: $(QPEDIR)/src/3rdparty/libraries/rsync/whole.c \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/config_linux.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/trace.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/fileutil.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/sumset.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/job.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/buf.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/whole.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/util.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/whole.o $(QPEDIR)/src/3rdparty/libraries/rsync/whole.c

$(OBJECTS_DIR)/qrsync.o: $(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.cpp \
		$(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.h \
		$(QPEDIR)/src/3rdparty/libraries/rsync/rsync.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/qrsync.o $(QPEDIR)/src/3rdparty/libraries/rsync/qrsync.cpp

$(OBJECTS_DIR)/syncdialog.o: syncdialog.cpp \
		syncdialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/syncdialog.o syncdialog.cpp

$(OBJECTS_DIR)/serverapp.o: serverapp.cpp \
		$(MOC_DIR)/serverapp.moc \
		serverapp.h \
		shutdownimpl.h \
		shutdown.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/serverapp.o serverapp.cpp

shutdown.h: shutdown.ui
	$(UIC) shutdown.ui -o $(INTERFACE_DECL_PATH)/shutdown.h

shutdown.cpp: shutdown.ui
	$(UIC) shutdown.ui -i shutdown.h -o shutdown.cpp

$(OBJECTS_DIR)/shutdown.o: shutdown.cpp \
		shutdown.h \
		shutdown.ui
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/shutdown.o shutdown.cpp

$(OBJECTS_DIR)/moc_server.o: $(MOC_DIR)/moc_server.cpp \
		server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_server.o $(MOC_DIR)/moc_server.cpp

$(OBJECTS_DIR)/moc_launchertab.o: $(MOC_DIR)/moc_launchertab.cpp \
		launchertab.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_launchertab.o $(MOC_DIR)/moc_launchertab.cpp

$(OBJECTS_DIR)/moc_documentlist.o: $(MOC_DIR)/moc_documentlist.cpp \
		documentlist.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_documentlist.o $(MOC_DIR)/moc_documentlist.cpp

$(OBJECTS_DIR)/moc_suspendmonitor.o: $(MOC_DIR)/moc_suspendmonitor.cpp \
		suspendmonitor.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_suspendmonitor.o $(MOC_DIR)/moc_suspendmonitor.cpp

$(OBJECTS_DIR)/moc_appicons.o: $(MOC_DIR)/moc_appicons.cpp \
		appicons.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_appicons.o $(MOC_DIR)/moc_appicons.cpp

$(OBJECTS_DIR)/moc_taskbar.o: $(MOC_DIR)/moc_taskbar.cpp \
		taskbar.h \
		serverinterface.h \
		startmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_taskbar.o $(MOC_DIR)/moc_taskbar.cpp

$(OBJECTS_DIR)/moc_runningappbar.o: $(MOC_DIR)/moc_runningappbar.cpp \
		runningappbar.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_runningappbar.o $(MOC_DIR)/moc_runningappbar.cpp

$(OBJECTS_DIR)/moc_applauncher.o: $(MOC_DIR)/moc_applauncher.cpp \
		applauncher.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_applauncher.o $(MOC_DIR)/moc_applauncher.cpp

$(OBJECTS_DIR)/moc_inputmethods.o: $(MOC_DIR)/moc_inputmethods.cpp \
		inputmethods.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_inputmethods.o $(MOC_DIR)/moc_inputmethods.cpp

$(OBJECTS_DIR)/moc_systray.o: $(MOC_DIR)/moc_systray.cpp \
		systray.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_systray.o $(MOC_DIR)/moc_systray.cpp

$(OBJECTS_DIR)/moc_shutdownimpl.o: $(MOC_DIR)/moc_shutdownimpl.cpp \
		shutdownimpl.h \
		shutdown.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_shutdownimpl.o $(MOC_DIR)/moc_shutdownimpl.cpp

$(OBJECTS_DIR)/moc_launcher.o: $(MOC_DIR)/moc_launcher.cpp \
		launcher.h \
		launcherview.h \
		launchertab.h \
		serverinterface.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_launcher.o $(MOC_DIR)/moc_launcher.cpp

$(OBJECTS_DIR)/moc_launcherview.o: $(MOC_DIR)/moc_launcherview.cpp \
		launcherview.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_launcherview.o $(MOC_DIR)/moc_launcherview.cpp

$(OBJECTS_DIR)/moc_calibrate.o: $(MOC_DIR)/moc_calibrate.cpp \
		$(QPEDIR)/src/settings/calibrate/calibrate.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_calibrate.o $(MOC_DIR)/moc_calibrate.cpp

$(OBJECTS_DIR)/moc_startmenu.o: $(MOC_DIR)/moc_startmenu.cpp \
		startmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_startmenu.o $(MOC_DIR)/moc_startmenu.cpp

$(OBJECTS_DIR)/moc_transferserver.o: $(MOC_DIR)/moc_transferserver.cpp \
		transferserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_transferserver.o $(MOC_DIR)/moc_transferserver.cpp

$(OBJECTS_DIR)/moc_qcopbridge.o: $(MOC_DIR)/moc_qcopbridge.cpp \
		qcopbridge.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_qcopbridge.o $(MOC_DIR)/moc_qcopbridge.cpp

$(OBJECTS_DIR)/moc_packageslave.o: $(MOC_DIR)/moc_packageslave.cpp \
		packageslave.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_packageslave.o $(MOC_DIR)/moc_packageslave.cpp

$(OBJECTS_DIR)/moc_irserver.o: $(MOC_DIR)/moc_irserver.cpp \
		irserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_irserver.o $(MOC_DIR)/moc_irserver.cpp

$(OBJECTS_DIR)/moc_firstuse.o: $(MOC_DIR)/moc_firstuse.cpp \
		firstuse.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_firstuse.o $(MOC_DIR)/moc_firstuse.cpp

$(OBJECTS_DIR)/moc_syncdialog.o: $(MOC_DIR)/moc_syncdialog.cpp \
		syncdialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_syncdialog.o $(MOC_DIR)/moc_syncdialog.cpp

$(OBJECTS_DIR)/moc_serverapp.o: $(MOC_DIR)/moc_serverapp.cpp \
		serverapp.h \
		shutdownimpl.h \
		shutdown.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_serverapp.o $(MOC_DIR)/moc_serverapp.cpp

$(OBJECTS_DIR)/moc_shutdown.o: $(MOC_DIR)/moc_shutdown.cpp \
		shutdown.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/moc_shutdown.o $(MOC_DIR)/moc_shutdown.cpp

$(MOC_DIR)/moc_server.cpp: server.h
	$(MOC) server.h -o $(MOC_DIR)/moc_server.cpp

$(MOC_DIR)/moc_launchertab.cpp: launchertab.h
	$(MOC) launchertab.h -o $(MOC_DIR)/moc_launchertab.cpp

$(MOC_DIR)/moc_documentlist.cpp: documentlist.h
	$(MOC) documentlist.h -o $(MOC_DIR)/moc_documentlist.cpp

$(MOC_DIR)/moc_suspendmonitor.cpp: suspendmonitor.h
	$(MOC) suspendmonitor.h -o $(MOC_DIR)/moc_suspendmonitor.cpp

$(MOC_DIR)/moc_appicons.cpp: appicons.h
	$(MOC) appicons.h -o $(MOC_DIR)/moc_appicons.cpp

$(MOC_DIR)/moc_taskbar.cpp: taskbar.h
	$(MOC) taskbar.h -o $(MOC_DIR)/moc_taskbar.cpp

$(MOC_DIR)/moc_runningappbar.cpp: runningappbar.h
	$(MOC) runningappbar.h -o $(MOC_DIR)/moc_runningappbar.cpp

$(MOC_DIR)/moc_applauncher.cpp: applauncher.h
	$(MOC) applauncher.h -o $(MOC_DIR)/moc_applauncher.cpp

$(MOC_DIR)/moc_inputmethods.cpp: inputmethods.h
	$(MOC) inputmethods.h -o $(MOC_DIR)/moc_inputmethods.cpp

$(MOC_DIR)/moc_systray.cpp: systray.h
	$(MOC) systray.h -o $(MOC_DIR)/moc_systray.cpp

$(MOC_DIR)/moc_shutdownimpl.cpp: shutdownimpl.h
	$(MOC) shutdownimpl.h -o $(MOC_DIR)/moc_shutdownimpl.cpp

$(MOC_DIR)/moc_launcher.cpp: launcher.h
	$(MOC) launcher.h -o $(MOC_DIR)/moc_launcher.cpp

$(MOC_DIR)/moc_launcherview.cpp: launcherview.h
	$(MOC) launcherview.h -o $(MOC_DIR)/moc_launcherview.cpp

$(MOC_DIR)/moc_calibrate.cpp: $(QPEDIR)/src/settings/calibrate/calibrate.h
	$(MOC) $(QPEDIR)/src/settings/calibrate/calibrate.h -o $(MOC_DIR)/moc_calibrate.cpp

$(MOC_DIR)/moc_startmenu.cpp: startmenu.h
	$(MOC) startmenu.h -o $(MOC_DIR)/moc_startmenu.cpp

$(MOC_DIR)/moc_transferserver.cpp: transferserver.h
	$(MOC) transferserver.h -o $(MOC_DIR)/moc_transferserver.cpp

$(MOC_DIR)/moc_qcopbridge.cpp: qcopbridge.h
	$(MOC) qcopbridge.h -o $(MOC_DIR)/moc_qcopbridge.cpp

$(MOC_DIR)/moc_packageslave.cpp: packageslave.h
	$(MOC) packageslave.h -o $(MOC_DIR)/moc_packageslave.cpp

$(MOC_DIR)/moc_irserver.cpp: irserver.h
	$(MOC) irserver.h -o $(MOC_DIR)/moc_irserver.cpp

$(MOC_DIR)/moc_firstuse.cpp: firstuse.h
	$(MOC) firstuse.h -o $(MOC_DIR)/moc_firstuse.cpp

$(MOC_DIR)/moc_syncdialog.cpp: syncdialog.h
	$(MOC) syncdialog.h -o $(MOC_DIR)/moc_syncdialog.cpp

$(MOC_DIR)/moc_serverapp.cpp: serverapp.h
	$(MOC) serverapp.h -o $(MOC_DIR)/moc_serverapp.cpp

$(MOC_DIR)/serverinterface.moc: serverinterface.cpp
	$(MOC) serverinterface.cpp -o $(MOC_DIR)/serverinterface.moc

$(MOC_DIR)/documentlist.moc: documentlist.cpp
	$(MOC) documentlist.cpp -o $(MOC_DIR)/documentlist.moc

$(MOC_DIR)/appicons.moc: appicons.cpp
	$(MOC) appicons.cpp -o $(MOC_DIR)/appicons.moc

$(MOC_DIR)/taskbar.moc: taskbar.cpp
	$(MOC) taskbar.cpp -o $(MOC_DIR)/taskbar.moc

$(MOC_DIR)/serverapp.moc: serverapp.cpp
	$(MOC) serverapp.cpp -o $(MOC_DIR)/serverapp.moc

$(MOC_DIR)/moc_shutdown.cpp: shutdown.h
	$(MOC) shutdown.h -o $(MOC_DIR)/moc_shutdown.cpp



lupdate-check:
	@sed -e '/$$$$QTOPIA1DIR[^ ]*/ d' \
	    -e '/$$(.*/ d' \
	    -e 's/^static:.*//' \
	    <server.pro >t$$$$.pro && \
	rm -rf t$$$$&& \
	mkdir t$$$$ && \
	cp -a *.ts t$$$$ && \
	chmod +w *.ts && \
	lupdate  t$$$$.pro && \
	rm t$$$$.pro && \
	for i in *.ts; \
	do \
	    if cmp -s $$i t$$$$/$$i; \
	    then \
		true; \
	    else \
		echo out-of-date $$i; \
	    fi; \
	    rm $$i; \
	    mv t$$$$/$$i $$i; \
	done && \
	rmdir t$$$$

lupdate:
	sed -e '/$$$$QTOPIA1DIR[^ ]*/ d' \
	    -e '/$$(.*/ d' \
	    -e 's/^static:.*//' \
	    <server.pro >t$$$$.pro && \
	lupdate  t$$$$.pro && \
	rm t$$$$.pro

lrelease:
	lrelease server.pro
	$(QPEDIR)/bin/qtopia-linstall server.pro


ipkg: $(DESTDIR)$(TARGET) # among other things...
	CTL="$(CONTROL)"; for ctrl in $$CTL; do cd $(QPEDIR)/ipkg; ../bin/mkipks -platform generic  $(QPEDIR)/src/$$ctrl ; done

package: ipkg
	cd $(QPEDIR)/ipkg; 	../bin/mkPackages

Makefile: server.pro
	cd $(QPEDIR)/src; 	./configure  -debug -make server
