DEFINES += OPIE USEQPE
VPATH = ..
TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= Aportis.h \
                  Bkmks.h \
                  BuffDoc.h \
                  ButtonPrefs.h \
                  CAnnoEdit.h \
                  CBuffer.h \
                  CDrawBuffer.h \
                  CEncoding.h \
                  CEncoding_tables.h \
                  CExpander.h \
                  CFilter.h \
                  CloseDialog.h \
                  Filedata.h \
                  FixedFont.h \
                  FontControl.h \
                  GraphicWin.h \
                  Markups.h \
                  Navigation.h \
                  Palm2QImage.h \
                  Prefs.h \
                  PPMd.h \
                  PPMdType.h \
                  QFloatBar.h \
                  QTReader.h \
                  QTReaderApp.h \
                  QtrListView.h \
                  Queue.h \
                  StyleConsts.h \
                  ToolbarPrefs.h \
                  URLDialog.h \
                  ZText.h \
                  arith.h \
                  cbkmkselector.h \
                  config.h \
                  fileBrowser.h \
                  infowin.h \
                  linktype.h \
                  my_list.h \
                  names.h \
                  pdb.h \
                  plucker.h \
                  plucker_base.h \
                  ppm.h \
                  ppm_expander.h \
                  usenef.h \
                  ustring.h \
		  util.h \
                  utypes.h \
                  version.h \
                  ztxt.h

SOURCES		= BuffDoc.cpp \
                  ButtonPrefs.cpp \
                  CAnnoEdit.cpp \
                  CDrawBuffer.cpp \
                  CEncoding.cpp \
                  CEncoding_tables.cpp \
                  CFilter.cpp \
		  CRegExp.cpp \
                  CloseDialog.cpp \
                  FontControl.cpp \
		  GraphicWin.cpp \
                  Prefs.cpp \
                  QTReader.cpp \
                  QTReaderApp.cpp \
                  QtrListView.cpp \
                  ToolbarPrefs.cpp \
                  URLDialog.cpp \
                  fileBrowser.cpp \
                  infowin.cpp \
                  main.cpp \
		  orkey.cpp \
		  util.cpp \
                  version.cpp

INTERFACES	= 
DESTDIR		= $(OPIEDIR)/bin
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
TARGET		= opie-reader
LIBS		+= -lopiecore2 -lopieui2 -lqpe -lreader_codec

include ( $(OPIEDIR)/include.pro )
