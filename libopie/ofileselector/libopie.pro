TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS   = ofontmenu.h \
    ocolorbutton.h \
    ofileselector/odefaultfactories.h \
    ofileselector/ofiledialog.h \
    ofileselector/ofilelistview.h \
    ofileselector/ofileselector.h \
    ofileselector/ofileselectoritem.h \
    ofileselector/ofileseleczormain.h \
    ofileselector/ofileview.h \
    ofileselector/olister.h \
    ofileselector/olocallister.h \
    ofileselector/opixmapprovider.h \
    tododb.h \
    ocheckitem.h todoevent.h todoresource.h \
    todovcalresource.h xmltree.h \
    colordialog.h colorpopupmenu.h \
    oclickablelabel.h oprocctrl.h \
    oprocess.h odevice.h \
    otimepicker.h otabwidget.h \
    otabbar.h otabinfo.h \
    ofontselector.h \
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
    pim/obackendfactory.h \
    pim/opimcache.h \ 
    pim/otodoaccessvcal.h \
    pim/orecur.h \
    orecurrancewidget.h
#    pim/otodoaccesssql.h \

SOURCES   = ofontmenu.cc  \
        ocolorbutton.cpp \
	ofileselector/odefaultfactories.cpp \
	ofileselector/ofiledialog.cpp \
	ofileselector/ofilefactory.cpp \
	ofileselector/ofilelistview.cpp \
	ofileselector/ofileselector.cpp \
	ofileselector/ofileselectoritem.cpp \
	ofileselector/ofileselectormain.cpp \
	ofileselector/ofileview.cpp \
	ofileselector/olister.cpp \
	ofileselector/olocallister.cpp \
	ofileselector/opixmapprovider.cpp \
	    xmltree.cc \
            ocheckitem.cpp tododb.cpp todoevent.cpp \
	    todovcalresource.cpp  colordialog.cpp \
	    colorpopupmenu.cpp oclickablelabel.cpp \
	    oprocctrl.cpp oprocess.cpp \
	    odevice.cpp otimepicker.cpp \
	     otabwidget.cpp otabbar.cpp \
             ofontselector.cpp \
             pim/otodo.cpp \
             pim/opimrecord.cpp \
	     pim/otodoaccess.cpp \
	     pim/otodoaccessbackend.cpp \
             pim/otodoaccessxml.cpp \
             pim/ocontact.cpp \
             pim/ocontactaccess.cpp \
	     pim/otodoaccessvcal.cpp \
	     pim/orecur.cpp \
	     orecurrancewidget.cpp
#             pim/otodoaccesssql.cpp \

TARGET    = opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(QTDIR)/lib$(PROJMAK)
#VERSION = 1.0.0

# LIBS            += -lopiesql

INTERFACES      = otimepickerbase.ui orecurrancebase.ui

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
