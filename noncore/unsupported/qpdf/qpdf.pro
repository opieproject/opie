TEMPLATE    =  app
CONFIG      *= qt embedded release warn_off
CONFIG      -= warn_on
SOURCES     =  xpdf/Array.cc \
               xpdf/BuiltinFont.cc \
               xpdf/BuiltinFontTables.cc \
               xpdf/CMap.cc \
               xpdf/Catalog.cc \
               xpdf/CharCodeToUnicode.cc \
               xpdf/Decrypt.cc \
               xpdf/Dict.cc \
               xpdf/Error.cc \
               xpdf/FontEncodingTables.cc \
               xpdf/Annot.cc \
               xpdf/Function.cc \
               xpdf/Gfx.cc \
               xpdf/GfxFont.cc \
               xpdf/GfxState.cc \
               xpdf/GlobalParams.cc \
               xpdf/Lexer.cc \
               xpdf/Link.cc \
               xpdf/NameToCharCode.cc \
               xpdf/Object.cc \
               xpdf/OutputDev.cc \
               xpdf/PDFDoc.cc \
               xpdf/Page.cc \
               xpdf/Parser.cc \
               xpdf/PSTokenizer.cc \
               xpdf/Stream.cc \
               xpdf/TextOutputDev.cc \
               xpdf/UnicodeMap.cc \
               xpdf/XRef.cc \	
               goo/GHash.cc \
               goo/GString.cc \
               goo/GList.cc \
               QOutputDev.cpp \
               QPEOutputDev.cpp \
               qpdf.cpp \
               qbusybar.cpp \
               gooStub.cpp
HEADERS     =  QOutputDev.h \
               QPEOutputDev.h \
               qbusybar.h \
               qpdf.h
INCLUDEPATH += . \
               .. \
               xpdf \
               $(OPIEDIR)/include \
               ../goo \ 
               goo
               
LIBS        += -L $(OPIEDIR)/lib -lqpe -lopie -lstdc++
DESTDIR      = $(OPIEDIR)/bin
TARGET       = qpdf

TRANSLATIONS = ../../../i18n/de/qpdf.ts \
	 ../../../i18n/da/qpdf.ts \
	 ../../../i18n/xx/qpdf.ts \
	 ../../../i18n/en/qpdf.ts \
	 ../../../i18n/es/qpdf.ts \
	 ../../../i18n/fr/qpdf.ts \
	 ../../../i18n/hu/qpdf.ts \
	 ../../../i18n/ja/qpdf.ts \
	 ../../../i18n/ko/qpdf.ts \
	 ../../../i18n/no/qpdf.ts \
	 ../../../i18n/pl/qpdf.ts \
	 ../../../i18n/pt/qpdf.ts \
	 ../../../i18n/pt_BR/qpdf.ts \
	 ../../../i18n/sl/qpdf.ts \
	 ../../../i18n/zh_CN/qpdf.ts \
	 ../../../i18n/it/qpdf.ts \
	 ../../../i18n/zh_TW/qpdf.ts
