TEMPLATE	= app
CONFIG		= qt warn_on release
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
DESTDIR		= $(OPIEDIR)/bin
TARGET		= drawpad

TRANSLATIONS	= ../../i18n/de/drawpad.ts
TRANSLATIONS	+= ../../i18n/en/drawpad.ts
TRANSLATIONS	+= ../../i18n/es/drawpad.ts
TRANSLATIONS	+= ../../i18n/fr/drawpad.ts
TRANSLATIONS	+= ../../i18n/hu/drawpad.ts
TRANSLATIONS	+= ../../i18n/ja/drawpad.ts
TRANSLATIONS	+= ../../i18n/ko/drawpad.ts
TRANSLATIONS	+= ../../i18n/no/drawpad.ts
TRANSLATIONS	+= ../../i18n/pl/drawpad.ts
TRANSLATIONS	+= ../../i18n/pt/drawpad.ts
TRANSLATIONS	+= ../../i18n/pt_BR/drawpad.ts
TRANSLATIONS	+= ../../i18n/sl/drawpad.ts
TRANSLATIONS	+= ../../i18n/zh_CN/drawpad.ts
TRANSLATIONS	+= ../../i18n/zh_TW/drawpad.ts
