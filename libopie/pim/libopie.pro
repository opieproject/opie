TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS   = ofontmenu.h \
    tododb.h \
    todoevent.h todoresource.h \
    todovcalresource.h xmltree.h \
    colordialog.h colorpopupmenu.h \
    oclickablelabel.h oprocctrl.h \
    oprocess.h odevice.h \
    otimepicker.h otabwidget.h \
    otabbar.h otabinfo.h \
    ofileselector/ofiledialog.h \
    ofileselector/ofilelistview.h \
    ofileselector/ofileselector.h \
    ofileselector/ofileselectoritem.h \
    ofileselector/ofileview.h \
    ofileselector/olister.h \
    ofileselector/olocallister.h \
    ofileselector/ofileselectormain.h \
    pim/opimrecord.h \
    pim/otodo.h \
    pim/orecordlist.h \
    pim/opimaccesstemplate.h \
    pim/opimaccessbackend.h \
    pim/otodoaccess.h \
    pim/otodacessbackend.h \
    pim/ocontact.h \
    pim/ocontactaccess.h \
    pim/ocontactaccessbackend.h \
    pim/ocontactaccessbackend_xml.h \
    pim/orecord.h
    
SOURCES   = ofontmenu.cc  \
	    xmltree.cc \
	    tododb.cpp todoevent.cpp \
	    todovcalresource.cpp  colordialog.cpp \
	    colorpopupmenu.cpp oclickablelabel.cpp \
	    oprocctrl.cpp oprocess.cpp \
	    odevice.cpp otimepicker.cpp \
	     otabwidget.cpp otabbar.cpp \
             ofileselector/ofiledialog.cpp \
             ofileselector/ofilelistview.cpp \
             ofileselector/ofileselector.cpp \
             ofileselector/ofileselectoritem.cpp \
             ofileselector/ofileview.cpp \
             ofileselector/olister.cpp \
             ofileselector/olocallister.cpp \
             ofileselector/ofileselectormain.cpp \
             pim/otodo.cpp \
             pim/opimrecord.cpp \
	     pim/otodoaccess.cpp \
	     pim/otodoaccessbackend.cpp \
             pim/ocontact.cpp \
             pim/ocontactaccess.cpp \
	     pim/orecord.cpp

TARGET    = opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0

INTERFACES      = otimepickerbase.ui

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
