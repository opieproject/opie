TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= Aportis.h \
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
                  name.h \
		  names.h \
                  opie.h \
                  pdb.h \
                  plucker.h \
		  plucker_base.h \
                  ppm.h \
                  ppm_expander.h \
                  ustring.h \
		  usenef.h \
		  useqpe.h \
                  utypes.h \
                  version.h \
                  ztxt.h

SOURCES		= Aportis.cpp \
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
