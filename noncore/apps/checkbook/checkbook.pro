TEMPLATE	= app
CONFIG		= qt warn_on release
HEADERS		= config.h \
		  qcheckbook.h \
		  qcheckdetails.h \
		  qcheckentry.h \
		  qcheckgraph.h \
		  qcheckmainmenu.h \
		  qcheckname.h \
		  qcheckview.h \
		  qrestrictedcombo.h \
		  qrestrictedline.h
SOURCES		= config.cpp \
		  main.cpp \
		  qcheckbook.cpp \
		  qcheckdetails.cpp \
		  qcheckentry.cpp \
		  qcheckgraph.cpp \
		  qcheckmainmenu.cpp \
		  qcheckname.cpp \
		  qcheckview.cpp \
		  qrestrictedcombo.cpp \
		  qrestrictedline.cpp
INTERFACES	= qcheckdetailsbase.ui \
		  qcheckentrybase.ui \
		  qcheckgraphbase.ui \
		  qcheckmmbase.ui \
		  qchecknamebase.ui \
		  qcheckviewbase.ui
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS		+= -lqpe
TARGET 		= checkbook
DESTDIR		= $(OPIEDIR)/bin

TRANSLATIONS = ../../../i18n/de/checkbook.ts \
	 ../../../i18n/en/checkbook.ts \
	 ../../../i18n/es/checkbook.ts \
	 ../../../i18n/fr/checkbook.ts \
	 ../../../i18n/hu/checkbook.ts \
	 ../../../i18n/ja/checkbook.ts \
	 ../../../i18n/ko/checkbook.ts \
	 ../../../i18n/no/checkbook.ts \
	 ../../../i18n/pl/checkbook.ts \
	 ../../../i18n/pt/checkbook.ts \
	 ../../../i18n/pt_BR/checkbook.ts \
	 ../../../i18n/sl/checkbook.ts \
	 ../../../i18n/zh_CN/checkbook.ts \
	 ../../../i18n/zh_TW/checkbook.ts \
   	 ../../../i18n/it/checkbook.ts
