#############################################################################
# Automatically generated from noncore/tools/remote/Makefile.in
# Build options from 
#############################################################################

# Compiling
INTERFACE_DECL_PATH 	= .
SYSCONF_CXX		= arm-linux-g++
SYSCONF_CC		= arm-linux-gcc
DASHCROSS		= -arm

# Compiling with support libraries
SYSCONF_CXXFLAGS_X11	= 
SYSCONF_CXXFLAGS_QT	= -I$(QTDIR)/include
SYSCONF_CXXFLAGS_OPENGL	= -I/usr/X11R6/include

# Compiling YACC output
SYSCONF_CXXFLAGS_YACC     = -Wno-unused -Wno-parentheses

# Linking with support libraries
SYSCONF_RPATH_X11	= 
SYSCONF_RPATH_QT	= -Wl,-rpath,$(QTDIR)/lib
SYSCONF_RPATH_OPENGL	= -Wl,-rpath,/usr/X11R6/lib

# Linking with support libraries
# X11
SYSCONF_LFLAGS_X11	= 
SYSCONF_LIBS_X11	= 
# Qt, Qt+OpenGL
SYSCONF_LFLAGS_QT	= -L$(QTDIR)/lib
SYSCONF_LIBS_QT		= -lqte$(QT_THREAD_SUFFIX)
SYSCONF_LIBS_QT_OPENGL	= 
# OpenGL
SYSCONF_LFLAGS_OPENGL	= -L/usr/X11R6/lib
SYSCONF_LIBS_OPENGL	= 
# Yacc
SYSCONF_LIBS_YACC	= 

# Linking applications
SYSCONF_LINK		= arm-linux-gcc
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
SYSCONF_LINK_SHLIB	= arm-linux-gcc
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
SYSCONF_AR		= arm-linux-ar cqs
SYSCONF_LINK_TARGET_STATIC = lib$(TARGET).a
SYSCONF_LINK_LIB_STATIC	= rm -f $(DESTDIR)$(SYSCONF_LINK_TARGET_STATIC) ; \
				 $(SYSCONF_AR) $(DESTDIR)$(SYSCONF_LINK_TARGET_STATIC) $(OBJECTS) $(OBJMOC) 
# Compiling application source
SYSCONF_CXXFLAGS	= -pipe -DQT_QWS_IPAQ -DQWS -fno-exceptions -fno-rtti -O2 -Wall -W
SYSCONF_CFLAGS		= -pipe -O2 -Wall -W
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
#############################################################################

####### Compiler, tools and options

CXX	=	$(SYSCONF_CXX) $(QT_CXX_MT)
CXXFLAGS=	$(SYSCONF_CXXFLAGS_QT) $(SYSCONF_CXXFLAGS)
CC	=	$(SYSCONF_CC) $(QT_C_MT)
CFLAGS	=	$(SYSCONF_CFLAGS)
INCPATH =	-I$(OPIEDIR)/include
LFLAGS	=	$(SYSCONF_LFLAGS_QT) $(SYSCONF_RPATH_QT) $(SYSCONF_LFLAGS) $(QT_LFLAGS_MT)
LIBS	=	$(SUBLIBS) -lqpe -lopie $(SYSCONF_LIBS_QT) $(SYSCONF_LIBS) $(SYSCONF_LIBS_QTAPP)
MOC	=	$(SYSCONF_MOC)
UIC	=	$(SYSCONF_UIC)

####### Target

DESTDIR = $(OPIEDIR)/bin/
VER_MAJ = 1
VER_MIN = 0
VER_PATCH = 0
TARGET	= remote
TARGET1 = lib$(TARGET).so.$(VER_MAJ)

####### Files

HEADERS =	remotetab.h \
		learntab.h \
		configtab.h \
		topgroup.h \
		dvdgroup.h \
		channelgroup.h \
		vcrgroup.h \
		buttondialog.h \
		topgroupconf.h \
		dvdgroupconf.h \
		channelgroupconf.h \
		vcrgroupconf.h \
		mainview.h \
		recorddialog.h \
		helptab.h
SOURCES =	remote.cpp \
		remotetab.cpp \
		learntab.cpp \
		configtab.cpp \
		topgroup.cpp \
		dvdgroup.cpp \
		channelgroup.cpp \
		vcrgroup.cpp \
		buttondialog.cpp \
		topgroupconf.cpp \
		dvdgroupconf.cpp \
		channelgroupconf.cpp \
		vcrgroupconf.cpp \
		mainview.cpp \
		recorddialog.cpp \
		helptab.cpp
OBJECTS =	remote.o \
		remotetab.o \
		learntab.o \
		configtab.o \
		topgroup.o \
		dvdgroup.o \
		channelgroup.o \
		vcrgroup.o \
		buttondialog.o \
		topgroupconf.o \
		dvdgroupconf.o \
		channelgroupconf.o \
		vcrgroupconf.o \
		mainview.o \
		recorddialog.o \
		helptab.o
INTERFACES =    
UICDECLS =      
UICIMPLS =      
SRCMOC	=	moc_remotetab.cpp \
		moc_learntab.cpp \
		moc_configtab.cpp \
		moc_buttondialog.cpp \
		moc_mainview.cpp \
		moc_recorddialog.cpp
OBJMOC	=	moc_remotetab.o \
		moc_learntab.o \
		moc_configtab.o \
		moc_buttondialog.o \
		moc_mainview.o \
		moc_recorddialog.o


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
	tmake remote.pro

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

remote.o: remote.cpp \
		$(OPIEDIR)/include/qpe/qpeapplication.h \
		$(OPIEDIR)/include/qpe/resource.h \
		$(OPIEDIR)/include/qpe/config.h \
		mainview.h \
		remotetab.h \
		topgroup.h \
		dvdgroup.h \
		channelgroup.h \
		vcrgroup.h \
		learntab.h \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h \
		configtab.h \
		buttondialog.h \
		channelgroupconf.h \
		dvdgroupconf.h \
		topgroupconf.h \
		vcrgroupconf.h \
		helptab.h

remotetab.o: remotetab.cpp \
		remotetab.h \
		$(OPIEDIR)/include/qpe/config.h \
		topgroup.h \
		dvdgroup.h \
		$(OPIEDIR)/include/qpe/resource.h \
		channelgroup.h \
		vcrgroup.h

learntab.o: learntab.cpp \
		learntab.h \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h

configtab.o: configtab.cpp \
		configtab.h \
		$(OPIEDIR)/include/qpe/config.h \
		buttondialog.h \
		channelgroupconf.h \
		dvdgroupconf.h \
		topgroupconf.h \
		vcrgroupconf.h

topgroup.o: topgroup.cpp \
		topgroup.h \
		$(OPIEDIR)/include/qpe/config.h

dvdgroup.o: dvdgroup.cpp \
		dvdgroup.h \
		$(OPIEDIR)/include/qpe/resource.h

channelgroup.o: channelgroup.cpp \
		channelgroup.h

vcrgroup.o: vcrgroup.cpp \
		vcrgroup.h \
		$(OPIEDIR)/include/qpe/resource.h

buttondialog.o: buttondialog.cpp \
		buttondialog.h

topgroupconf.o: topgroupconf.cpp \
		topgroupconf.h \
		$(OPIEDIR)/include/qpe/config.h

dvdgroupconf.o: dvdgroupconf.cpp \
		dvdgroupconf.h

channelgroupconf.o: channelgroupconf.cpp \
		channelgroupconf.h

vcrgroupconf.o: vcrgroupconf.cpp \
		vcrgroupconf.h

mainview.o: mainview.cpp \
		mainview.h \
		$(OPIEDIR)/include/qpe/qpeapplication.h \
		$(OPIEDIR)/include/qpe/resource.h \
		$(OPIEDIR)/include/qpe/config.h \
		remotetab.h \
		topgroup.h \
		dvdgroup.h \
		channelgroup.h \
		vcrgroup.h \
		learntab.h \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h \
		configtab.h \
		buttondialog.h \
		channelgroupconf.h \
		dvdgroupconf.h \
		topgroupconf.h \
		vcrgroupconf.h \
		helptab.h

recorddialog.o: recorddialog.cpp \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h

helptab.o: helptab.cpp \
		helptab.h

moc_remotetab.o: moc_remotetab.cpp \
		remotetab.h \
		$(OPIEDIR)/include/qpe/config.h \
		topgroup.h \
		dvdgroup.h \
		$(OPIEDIR)/include/qpe/resource.h \
		channelgroup.h \
		vcrgroup.h

moc_learntab.o: moc_learntab.cpp \
		learntab.h \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h

moc_configtab.o: moc_configtab.cpp \
		configtab.h \
		$(OPIEDIR)/include/qpe/config.h \
		buttondialog.h \
		channelgroupconf.h \
		dvdgroupconf.h \
		topgroupconf.h \
		vcrgroupconf.h

moc_buttondialog.o: moc_buttondialog.cpp \
		buttondialog.h

moc_mainview.o: moc_mainview.cpp \
		mainview.h \
		$(OPIEDIR)/include/qpe/qpeapplication.h \
		$(OPIEDIR)/include/qpe/resource.h \
		$(OPIEDIR)/include/qpe/config.h \
		remotetab.h \
		topgroup.h \
		dvdgroup.h \
		channelgroup.h \
		vcrgroup.h \
		learntab.h \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h \
		configtab.h \
		buttondialog.h \
		channelgroupconf.h \
		dvdgroupconf.h \
		topgroupconf.h \
		vcrgroupconf.h \
		helptab.h

moc_recorddialog.o: moc_recorddialog.cpp \
		recorddialog.h \
		$(OPIEDIR)/include/opie/oprocess.h

moc_remotetab.cpp: remotetab.h
	$(MOC) remotetab.h -o moc_remotetab.cpp

moc_learntab.cpp: learntab.h
	$(MOC) learntab.h -o moc_learntab.cpp

moc_configtab.cpp: configtab.h
	$(MOC) configtab.h -o moc_configtab.cpp

moc_buttondialog.cpp: buttondialog.h
	$(MOC) buttondialog.h -o moc_buttondialog.cpp

moc_mainview.cpp: mainview.h
	$(MOC) mainview.h -o moc_mainview.cpp

moc_recorddialog.cpp: recorddialog.h
	$(MOC) recorddialog.h -o moc_recorddialog.cpp



lupdate:
	lupdate remote.pro

lrelease:
	lrelease remote.pro

