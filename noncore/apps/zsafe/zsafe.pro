CONFIG       = qt quick-app
HEADERS      = zsafe.h krc2.h category.h categorylist.h zlistview.h \
               scqtfiledlg.h
SOURCES      = main.cpp zsafe.cpp krc2.cpp category.cpp \
               categorylist.cpp zlistview.cpp shadedlistitem.cpp\
               scqtfileedit.cpp scqtfileedit.moc.cpp \
               scqtfiledlg.cpp
INTERFACES   = newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui infoform.ui wait.ui
INCLUDEPATH  += $(OPIEDIR)/include
DEPENDPATH   += $(OPIEDIR)/include
LIBS         += -lqpe -lopiecore2 -lopieui2
TARGET       = zsafe

include( $(OPIEDIR)/include.pro )

QWS {
message( "Configuring for multi-threaded Qt..." )

}

win32 {
    TEMPLATE = app
    DEFINES += DESKTOP QT_DLL QT_THREAD_SUPPORT
    SOURCES += extra/qsettings.cpp
    HEADERS += extra/qsettings.h
    INCLUDEPATH	+= extra
    DEPENDPATH += extra
    RC_FILE = zsafe.rc
}
