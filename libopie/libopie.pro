TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS   = ofontmenu.h \
    ocolorbutton.h \
    ofiledialog.h ofileselector.h \
    ofileselector_p.h \
    ocheckitem.h  \
    xmltree.h \
    colordialog.h colorpopupmenu.h \
    oclickablelabel.h oprocctrl.h \
    oprocess.h odevice.h odevicebutton.h \
    otimepicker.h otabwidget.h \
    otabbar.h otabinfo.h \
    ofontselector.h \
    pim/opimrecord.h \
    pim/otodo.h \
    pim/orecordlist.h \
    pim/opimaccesstemplate.h \
    pim/opimaccessbackend.h \
    pim/otodoaccess.h \
    pim/otodoaccessbackend.h \
    pim/oconversion.h \
    pim/ocontact.h \
    pim/ocontactfields.h \
    pim/ocontactaccess.h \
    pim/ocontactaccessbackend.h \
    pim/ocontactaccessbackend_xml.h \
    pim/ocontactaccessbackend_vcard.h \
    pim/obackendfactory.h \
    pim/opimcache.h \ 
    pim/otodoaccessvcal.h \
    pim/orecur.h \
    pim/opimstate.h \
    pim/opimxrefpartner.h \
    pim/opimxref.h \
    pim/opimxrefmanager.h \
    pim/opimmaintainer.h \
    pim/opimnotify.h \
    pim/opimnotifymanager.h \
    pim/opimmainwindow.h \
    pim/opimresolver.h \
    pim/oevent.h \
    pim/otimezone.h \
    pim/odatebookaccess.h \
    pim/odatebookaccessbackend.h \
    pim/odatebookaccessbackend_xml.h \
    orecurrancewidget.h \
    oticker.h owait.h

SOURCES   = ofontmenu.cc  \
        ocolorbutton.cpp \
        sharp_compat.cpp \
      xmltree.cc \
      ofiledialog.cc ofileselector.cpp \
            ocheckitem.cpp  \
      colordialog.cpp \
      colorpopupmenu.cpp oclickablelabel.cpp \
      oprocctrl.cpp oprocess.cpp \
      odevice.cpp odevicebutton.cpp otimepicker.cpp \
       otabwidget.cpp otabbar.cpp \
             ofontselector.cpp \
             pim/otodo.cpp \
             pim/opimrecord.cpp \
       pim/otodoaccess.cpp \
       pim/otodoaccessbackend.cpp \
             pim/otodoaccessxml.cpp \
             pim/oconversion.cpp \
             pim/ocontact.cpp \
             pim/ocontactfields.cpp \
             pim/ocontactaccess.cpp \
             pim/ocontactaccessbackend_vcard.cpp \
             pim/ocontactaccessbackend_xml.cpp \
       pim/otodoaccessvcal.cpp \
       pim/orecur.cpp \
       pim/opimstate.cpp \
       pim/opimxrefpartner.cpp \
       pim/opimxref.cpp \
       pim/opimxrefmanager.cpp \
       pim/opimmaintainer.cpp \
       pim/opimnotify.cpp \
       pim/opimnotifymanager.cpp \
       pim/opimmainwindow.cpp \
       pim/opimresolver.cpp \
       pim/oevent.cpp \
       pim/otimezone.cpp \
       pim/odatebookaccess.cpp \
       pim/odatebookaccessbackend.cpp \
       pim/odatebookaccessbackend_xml.cpp \
       orecurrancewidget.cpp \
       oticker.cpp owait.cpp 

TARGET    = opie
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib$(PROJMAK)

LIBS += -lqpe

# Add SQL-Support if selected by config (eilers) 
CONFTEST = $$system( echo $CONFIG_SQL_PIM_BACKEND )
contains( CONFTEST, y ){

DEFINES += __USE_SQL
LIBS    += -lopiedb2
HEADERS += pim/otodoaccesssql.h pim/ocontactaccessbackend_sql.h pim/odatebookaccessbackend_sql.h
SOURCES += pim/otodoaccesssql.cpp pim/ocontactaccessbackend_sql.cpp pim/odatebookaccessbackend_sql.cpp

}

INTERFACES      = otimepickerbase.ui orecurrancebase.ui
TARGET          = opie


include ( $(OPIEDIR)/include.pro )
