TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= Aportis.h \
		  BuffDoc.h \
		  CBuffer.h \
		  CExpander.h \
		  CFilter.h \
		  QTReader.h \
		  QTReaderApp.h \
		  Text.h \
		  ZText.h \
		  arith.h \
		  my_list.h \
		  ppm.h \
		  ppm_expander.h \
                  cbkmkselector.h \
		  fileBrowser.h \
		  ztxt.h \
		  QtrListView.h \
		  infowin.h \
		  version.h \
		  pdb.h \
		  utypes.h \
		  ustring.h \
		  CEncoding.h \
		  config.h
SOURCES		= Aportis.cpp \
		  BuffDoc.cpp \
		  CBuffer.cpp \
		  QTReader.cpp \
		  QTReaderApp.cpp \
		  arith_d.cpp \
		  main.cpp \
		  ppm.cpp \
		  ppm_expander.cpp \
		  ztxt.cpp \
		  QtrListView.cpp \
		  infowin.cpp \
		  pdb.cpp \
		  CEncoding.cpp \
		  fileBrowser.cpp
INTERFACES	= 
DESTDIR		= $(OPIEDIR)/bin
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
TARGET		= reader
LIBS		+= -lqpe

