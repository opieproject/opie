CONFIG		= qt warn_on release quick-app
HEADERS		= drawpad.h \
		  drawpadcanvas.h \
		  ellipsetool.h \
		  erasetool.h \
		  exportdialog.h \
		  filltool.h \
		  filledellipsetool.h \
		  filledrectangletool.h \
		  importdialog.h \
		  linetool.h \
		  newpagedialog.h \
		  page.h \
		  pageinformationdialog.h \
		  pointtool.h \
		  rectangletool.h \
		  shapetool.h \
		  texttool.h \
		  thumbnailview.h \
		  tool.h
SOURCES		= drawpad.cpp \
		  drawpadcanvas.cpp \
		  ellipsetool.cpp \
		  erasetool.cpp \
		  exportdialog.cpp \
		  filltool.cpp \
		  filledellipsetool.cpp \
		  filledrectangletool.cpp \
		  importdialog.cpp \
		  linetool.cpp \
		  main.cpp \
		  newpagedialog.cpp \
		  page.cpp \
		  pageinformationdialog.cpp \
		  pointtool.cpp \
		  rectangletool.cpp \
		  shapetool.cpp \
		  texttool.cpp \
		  thumbnailview.cpp \
		  tool.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS		+= -lqpe -lopie

TARGET		= drawpad

TRANSLATIONS = ../../../i18n/de/drawpad.ts \
	 ../../../i18n/nl/drawpad.ts \
	 ../../../i18n/da/drawpad.ts \
	 ../../../i18n/xx/drawpad.ts \
	 ../../../i18n/en/drawpad.ts \
	 ../../../i18n/es/drawpad.ts \
	 ../../../i18n/fr/drawpad.ts \
	 ../../../i18n/hu/drawpad.ts \
	 ../../../i18n/ja/drawpad.ts \
	 ../../../i18n/ko/drawpad.ts \
	 ../../../i18n/no/drawpad.ts \
	 ../../../i18n/pl/drawpad.ts \
	 ../../../i18n/pt/drawpad.ts \
	 ../../../i18n/pt_BR/drawpad.ts \
	 ../../../i18n/sl/drawpad.ts \
	 ../../../i18n/zh_CN/drawpad.ts \
	 ../../../i18n/it/drawpad.ts \
	 ../../../i18n/zh_TW/drawpad.ts

include ( $(OPIEDIR)/include.pro )
