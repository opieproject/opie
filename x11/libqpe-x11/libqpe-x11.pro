TEMPLATE	= lib
CONFIG		+= qt warn_on release
HEADERS	=   ../../library/calendar.h \
	    ../../library/global.h \
	    ../../library/resource.h \
	    ../../library/xmlreader.h \
	    ../../library/mimetype.h \
	    ../../library/menubutton.h \
	    ../../library/network.h \
	    ../../library/networkinterface.h \
	    ../../library/filemanager.h \
	    ../../library/qdawg.h \
	    ../../library/datebookmonth.h \
	    ../../library/fileselector.h \
	    ../../library/fileselector_p.h \
	    ../../library/imageedit.h \
	    ../../library/qcopenvelope_qws.h \
	    ../../library/qpestyle.h \
	    ../../library/qpedialog.h \
	    ../../library/lightstyle.h \
	    ../../library/config.h \
	    ../../library/applnk.h \
	    ../../library/sound.h \
	    ../../library/tzselect.h \
	    ../../library/qmath.h \
	    ../../library/datebookdb.h \
	    ../../library/alarmserver.h \
	    ../../library/process.h \
	    ../../library/password.h \
	    ../../library/timestring.h \
	    ../../library/power.h \
	    ../../library/storage.h \
	    ../../library/qpemessagebox.h \
	    ../../library/timeconversion.h \
	    ../../library/qpedebug.h \
	    ../../library/qpemenubar.h \
	    ../../library/qpetoolbar.h \
	    ../../library/backend/categories.h \
	    ../../library/stringutil.h \
	    ../../library/backend/palmtoprecord.h \
            ../../library/backend/task.h \
	    ../../library/backend/event.h \
	    ../../library/backend/contact.h\
	    ../../library/categorymenu.h \
	    ../../library/categoryedit_p.h \
	    ../../library/categoryselect.h \
	    ../../library/categorywidget.h \
            ../../library/ir.h \
            ../../library/backend/vobject_p.h \
            ../../library/findwidget_p.h \
            ../../library/finddialog.h \
            ../../library/lnkproperties.h \
            ../../library/windowdecorationinterface.h \
            ../../library/textcodecinterface.h \
            ../../library/imagecodecinterface.h \
            qt/qcopchannel_qws.h \
	    qpe/qpeapplication.h \
	    qpe/fontmanager.h \
            ../ipc/client/ocopclient.h \
            ../ipc/common/ocoppacket.h


SOURCES	=   ../../library/calendar.cpp \
	    ../../library/global.cpp \
	    ../../library/xmlreader.cpp \
	    ../../library/mimetype.cpp \
	    ../../library/menubutton.cpp \
	    ../../library/network.cpp \
	    ../../library/networkinterface.cpp \
	    ../../library/filemanager.cpp \
	    ../../library/qdawg.cpp \
	    ../../library/datebookmonth.cpp \
	    ../../library/fileselector.cpp \
	    ../../library/imageedit.cpp \
	    ../../library/resource.cpp \
	    ../../library/qcopenvelope_qws.cpp \
	    ../../library/qpestyle.cpp \
	    ../../library/qpedialog.cpp \
	    ../../library/lightstyle.cpp \
	    ../../library/config.cpp \
	    ../../library/applnk.cpp \
	    ../../library/sound.cpp \
	    ../../library/tzselect.cpp \
	    ../../library/qmath.c \
	    ../../library/datebookdb.cpp \
	    ../../library/alarmserver.cpp \
	    ../../library/password.cpp \
	    ../../library/process.cpp \
	    ../../library/process_unix.cpp \
	    ../../library/timestring.cpp \
	    ../../library/power.cpp \
	    ../../library/storage.cpp \
	    ../../library/qpemessagebox.cpp \
            ../../library/backend/timeconversion.cpp \
	    ../../library/qpedebug.cpp \
	    ../../library/qpemenubar.cpp \
	    ../../library/qpetoolbar.cpp \
	    ../../library/backend/categories.cpp \
            ../../library/backend/stringutil.cpp \
	    ../../library/backend/palmtoprecord.cpp \
            ../../library/backend/task.cpp \
	    ../../library/backend/event.cpp \
	    ../../library/backend/contact.cpp \
	    ../../library/categorymenu.cpp \
            ../../library/categoryedit_p.cpp \
            ../../library/categoryselect.cpp \
            ../../library/categorywidget.cpp \
            ../../library/ir.cpp \
            ../../library/backend/vcc_yacc.cpp \
            ../../library/backend/vobject.cpp \
            ../../library/findwidget_p.cpp \
            ../../library/finddialog.cpp \
            ../../library/lnkproperties.cpp \
            qt/qcopchannel_qws.cpp \
            qpe/qpeapplication.cpp \
            ../ipc/client/ocopclient.cpp \
            ../ipc/common/ocoppacket.cpp

# Qt 3 compatibility
HEADERS +=  ../../library/quuid.h \
             ../../library/qcom.h \
             ../../library/qlibrary.h  \
              ../../library/qlibrary_p.h

SOURCES +=  ../../library/quuid.cpp \
            ../../library/qlibrary.cpp \
            ../../library/qlibrary_unix.cpp

INCLUDEPATH += $(OPIEDIR)/include backend
LIBS		+= -ldl -lcrypt -lm

INTERFACES =  ../../library/passwordbase_p.ui \
              ../../library/categoryeditbase_p.ui \
              ../../library/findwidgetbase_p.ui \
              ../../library/lnkpropertiesbase_p.ui

TARGET		= qpe
DESTDIR		= $(QTDIR)/lib$(PROJMAK)
# This is set by configure$(OPIEDIR).
VERSION		= 1.5.0.1

