#############################################################################
# Automatically generated from noncore/advancedfm/Makefile.in
# Build options from 
#############################################################################

# Compiling
INTERFACE_DECL_PATH 	= .
SYSCONF_CXX		= g++
SYSCONF_CC		= gcc
DASHCROSS		= 

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
SYSCONF_CXXFLAGS	= -pipe -DQWS -fno-exceptions -fno-rtti -O2 -Wall -W
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
LIBS	=	$(SUBLIBS) -lqpe $(SYSCONF_LIBS_QT) $(SYSCONF_LIBS) $(SYSCONF_LIBS_QTAPP)
MOC	=	$(SYSCONF_MOC)
UIC	=	$(SYSCONF_UIC)

####### Target

DESTDIR = $(OPIEDIR)/bin/
VER_MAJ = 1
VER_MIN = 0
VER_PATCH = 0
TARGET	= advancedfm
TARGET1 = lib$(TARGET).so.$(VER_MAJ)

####### Files

HEADERS =	advancedfm.h \
		inputDialog.h \
		filePermissions.h \
		output.h
SOURCES =	advancedfm.cpp \
		inputDialog.cpp \
		filePermissions.cpp \
		output.cpp \
		main.cpp
OBJECTS =	advancedfm.o \
		inputDialog.o \
		filePermissions.o \
		output.o \
		main.o
INTERFACES =    
UICDECLS =      
UICIMPLS =      
SRCMOC	=	moc_advancedfm.cpp \
		moc_inputDialog.cpp \
		moc_filePermissions.cpp \
		moc_output.cpp
OBJMOC	=	moc_advancedfm.o \
		moc_inputDialog.o \
		moc_filePermissions.o \
		moc_output.o


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
	tmake advancedfm.pro

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


REQUIRES=medium-config

####### Sub-libraries


###### Combined headers



####### Compile

advancedfm.o: advancedfm.cpp \
		advancedfm.h \
		inputDialog.h \
		filePermissions.h \
		output.h \
		$(OPIEDIR)/include/qpe/lnkproperties.h \
		$(OPIEDIR)/include/qpe/filemanager.h \
		$(OPIEDIR)/include/qpe/qcopenvelope_qws.h \
		$(OPIEDIR)/include/qpe/qpemenubar.h \
		$(OPIEDIR)/include/qpe/qpetoolbar.h \
		$(OPIEDIR)/include/qpe/qpeapplication.h \
		$(OPIEDIR)/include/qpe/resource.h \
		$(OPIEDIR)/include/qpe/config.h \
		$(OPIEDIR)/include/qpe/mimetype.h \
		$(OPIEDIR)/include/qpe/applnk.h

inputDialog.o: inputDialog.cpp \
		inputDialog.h

filePermissions.o: filePermissions.cpp \
		filePermissions.h

output.o: output.cpp \
		output.h

main.o: main.cpp \
		$(OPIEDIR)/include/qpe/qpeapplication.h \
		$(OPIEDIR)/include/qpe/qcopenvelope_qws.h \
		$(OPIEDIR)/include/qpe/config.h \
		advancedfm.h

moc_advancedfm.o: moc_advancedfm.cpp \
		advancedfm.h

moc_inputDialog.o: moc_inputDialog.cpp \
		inputDialog.h

moc_filePermissions.o: moc_filePermissions.cpp \
		filePermissions.h

moc_output.o: moc_output.cpp \
		output.h

moc_advancedfm.cpp: advancedfm.h
	$(MOC) advancedfm.h -o moc_advancedfm.cpp

moc_inputDialog.cpp: inputDialog.h
	$(MOC) inputDialog.h -o moc_inputDialog.cpp

moc_filePermissions.cpp: filePermissions.h
	$(MOC) filePermissions.h -o moc_filePermissions.cpp

moc_output.cpp: output.h
	$(MOC) output.h -o moc_output.cpp



lupdate:
	lupdate advancedfm.pro

lrelease:
	lrelease advancedfm.pro

