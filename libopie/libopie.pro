TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= ofontmenu.h ofileselector.h ofiledialog.h ofileview.h tododb.h todoevent.h todoresource.h todovcalresource.h xmltree.h  colordialog.h colorpopupmenu.h oclickablelabel.h oprocctrl.h oprocess.h odevice.h ocheckitem.h ocontactdb.h ocontactselector.h
SOURCES		= ofontmenu.cc ofileselector.cc ofiledialog.cc xmltree.cc tododb.cpp todoevent.cpp todovcalresource.cpp  colordialog.cpp colorpopupmenu.cpp oclickablelabel.cpp oprocctrl.cpp oprocess.cpp odevice.cpp ocheckitem.cpp ocontactdb.cpp ocontactselector.cpp
TARGET		= opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR         = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0

TRANSLATIONS = ../i18n/de/libopie.ts \
	 ../i18n/en/libopie.ts \
	 ../i18n/es/libopie.ts \
	 ../i18n/fr/libopie.ts \
	 ../i18n/hu/libopie.ts \
	 ../i18n/ja/libopie.ts \
	 ../i18n/ko/libopie.ts \
	 ../i18n/no/libopie.ts \
	 ../i18n/pl/libopie.ts \
	 ../i18n/pt/libopie.ts \
	 ../i18n/pt_BR/libopie.ts \
	 ../i18n/sl/libopie.ts \
	 ../i18n/zh_CN/libopie.ts \
	 ../i18n/zh_TW/libopie.ts
