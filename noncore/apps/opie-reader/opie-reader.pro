CONFIG      = qt warn_on
HEADERS     = Aportis.h \
                  Bkmks.h \
                  BuffDoc.h \
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
                  QFloatBar.h \
                  QTReader.h \
                  QTReaderApp.h \
                  QtrListView.h \
                  Queue.h \
                  StateData.h \
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
                  opie.h \
                  pdb.h \
                  plucker_base.h \
                  plucker.h \
                  ppm.h \
                  ppm_expander.h \
                  usenef.h \
                  useqpe.h \
                  ustring.h \
                  utypes.h \
                  version.h \
                  ztxt.h

SOURCES     = Aportis.cpp \
                  Bkmks.cpp \
                  BuffDoc.cpp \
                  CBuffer.cpp \
                  CDrawBuffer.cpp \
                  CEncoding.cpp \
                  CEncoding_tables.cpp \
                  CFilter.cpp \
                  CloseDialog.cpp \
                  FontControl.cpp \
                  Navigation.cpp \
                  Palm2QImage.cpp \
                  Prefs.cpp \
                  QTReader.cpp \
                  QTReaderApp.cpp \
                  QtrListView.cpp \
                  StyleConsts.cpp \
                  ToolbarPrefs.cpp \
                  URLDialog.cpp \
                  arith_d.cpp \
                  fileBrowser.cpp \
                  infowin.cpp \
                  main.cpp \
                  pdb.cpp \
                  plucker.cpp \
                  plucker_base.cpp \
                  ppm.cpp \
                  ppm_expander.cpp \
                  version.cpp \
                  ztxt.cpp

DESTDIR = $(OPIEDIR)/bin
TARGET = opie-reader

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )


