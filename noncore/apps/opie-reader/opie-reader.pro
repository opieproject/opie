TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= Aportis.h \
                  Bkmks.h \
                  BuffDoc.h \
                  CAnnoEdit.h \
                  CBuffer.h \
                  CDrawBuffer.h \
                  CEncoding.h \
                  CExpander.h \
                  CFilter.h \
                  Filedata.h \
                  FontControl.h \
                  GraphicWin.h \
                  Markups.h \
                  Navigation.h \
                  Palm2QImage.h \
                  QFloatBar.h \
                  QTReader.h \
                  QTReaderApp.h \
                  QtrListView.h \
                  Queue.h \
                  StateData.h \
                  StyleConsts.h \
                  ZText.h \
                  arith.h \
                  cbkmkselector.h \
                  config.h \
                  fileBrowser.h \
                  infowin.h \
                  my_list.h \
                  name.h \
                  opie.h \
                  pdb.h \
                  plucker.h \
                  ppm.h \
                  ppm_expander.h \
                  ustring.h \
                  utypes.h \
                  version.h \
                  ztxt.h

SOURCES		= Aportis.cpp \
                  Bkmks.cpp \
                  BuffDoc.cpp \
                  CBuffer.cpp \
                  CDrawBuffer.cpp \
                  CEncoding.cpp \
                  CFilter.cpp \
                  FontControl.cpp \
                  Navigation.cpp \
                  Palm2QImage.cpp \
                  QTReader.cpp \
                  QTReaderApp.cpp \
                  QtrListView.cpp \
                  StyleConsts.cpp \
                  arith_d.cpp \
                  fileBrowser.cpp \
                  infowin.cpp \
                  main.cpp \
                  pdb.cpp \
                  plucker.cpp \
                  ppm.cpp \
                  ppm_expander.cpp \
                  ztxt.cpp

INTERFACES	= 
DESTDIR		=  $(OPIEDIR)/bin
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
TARGET		=  reader
LIBS		+= -lqpe

