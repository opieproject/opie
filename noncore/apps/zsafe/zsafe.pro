TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
HEADERS         = zsafe.h krc2.h category.h categorylist.h zlistview.h \
                  scqtfiledlg.h 
SOURCES         = main.cpp zsafe.cpp krc2.cpp category.cpp \
                  categorylist.cpp zlistview.cpp shadedlistitem.cpp\
                  scqtfileedit.cpp scqtfileedit.moc.cpp \
                  scqtfiledlg.cpp
INCLUDEPATH	+= $(QPEDIR)/include
INCLUDEPATH	+= .
DEPENDPATH	+= $(QPEDIR)/include
LIBS            += -lqpe
#INTERFACES	= newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui scqtfiledlg.ui
INTERFACES	= newdialog.ui searchdialog.ui passworddialog.ui categorydialog.ui 
INTERFACES      += infoform.ui wait.ui
TARGET		= zsafe
#DESTDIR        = ../bin
TRANSLATIONS    = i18n/de/zsafe.ts
