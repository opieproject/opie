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


TRANSLATIONS = ../../../i18n/de/reader.ts \
	 ../../../i18n/nl/reader.ts \
         ../../../i18n/da/reader.ts \
         ../../../i18n/xx/reader.ts \
   ../../../i18n/en/reader.ts \
   ../../../i18n/es/reader.ts \
   ../../../i18n/fr/reader.ts \
   ../../../i18n/hu/reader.ts \
   ../../../i18n/ja/reader.ts \
   ../../../i18n/ko/reader.ts \
   ../../../i18n/no/reader.ts \
   ../../../i18n/pl/reader.ts \
   ../../../i18n/pt/reader.ts \
   ../../../i18n/pt_BR/reader.ts \
   ../../../i18n/sl/reader.ts \
   ../../../i18n/zh_CN/reader.ts \
   ../../../i18n/zh_TW/reader.ts


include ( $(OPIEDIR)/include.pro )
