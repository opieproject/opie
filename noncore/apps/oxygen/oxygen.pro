TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= oxygen.h \
            kmolcalc.h \
            kmolelements.h \
            calcdlgui.h \
            dataTable.h \
            psewidget.h \
            oxyframe.h \
            datawidgetui.h

SOURCES		= main.cpp \
		  oxygen.cpp \
          kmolcalc.cpp \
          calcdlgui.cpp \
          kmolelements.cpp \
          dataTable.cpp \
          psewidget.cpp \
          oxyframe.cpp \
          datawidgetui.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe -lstdc++
INTERFACES	= calcdlg.ui 
TARGET 		= oxygen
DESTDIR		= $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/oxygen.ts \
	 ../../../i18n/en/oxygen.ts \
	 ../../../i18n/es/oxygen.ts \
	 ../../../i18n/fr/oxygen.ts \
	 ../../../i18n/hu/oxygen.ts \
	 ../../../i18n/ja/oxygen.ts \
	 ../../../i18n/ko/oxygen.ts \
	 ../../../i18n/no/oxygen.ts \
	 ../../../i18n/pl/oxygen.ts \
	 ../../../i18n/pt/oxygen.ts \
	 ../../../i18n/pt_BR/oxygen.ts \
	 ../../../i18n/sl/oxygen.ts \
	 ../../../i18n/zh_CN/oxygen.ts \
	 ../../../i18n/zh_TW/oxygen.ts \
   	 ../../../i18n/it/oxygen.ts \
   	 ../../../i18n/da/oxygen.ts
