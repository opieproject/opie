CONFIG += qt warn_on quick-app
DESTDIR = $(OPIEDIR)/bin
TEMPLATE = app
TARGET = opie-eye
# the name of the resulting object

HEADERS = gui/iconview.h gui/filesystem.h gui/mainwindow.h \
	  lib/imagecache.h impl/dir/dir_dirview.h \
	  iface/dirview.h iface/dirlister.h iface/ifaceinfo.h \
	  impl/dir/dir_lister.h impl/dir/dir_ifaceinfo.h \
	  lib/slavemaster.h \
	  iface/slaveiface.h \
          gui/imageinfoui.h \
          gui/imageview.h \
	  gui/viewmodebutton.h \
      impl/doc/doc_lister.h impl/doc/doc_dirview.h \
      impl/doc/doc_ifaceinfo.h
	
# A list header files


SOURCES = gui/iconview.cpp gui/filesystem.cpp gui/mainwindow.cpp \
          lib/imagecache.cpp lib/viewmap.cpp  \
	  impl/dir/dir_dirview.cpp iface/dirlister.cpp \
	  iface/dirview.cpp impl/dir/dir_lister.cpp \
	  impl/dir/dir_ifaceinfo.cpp lib/slavemaster.cpp \
          gui/imageinfoui.cpp \
          gui/imageview.cpp \
	  gui/viewmodebutton.cpp \
      impl/doc/doc_lister.cpp impl/doc/doc_dirview.cpp \
      impl/doc/doc_ifaceinfo.cpp
# A list of source files

INTERFACES = 
# list of ui files

INCLUDEPATH += . $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include



LIBS += -lqpe -lopiecore2 -lopieui2 -lopiemm2

include ( $(OPIEDIR)/include.pro )
