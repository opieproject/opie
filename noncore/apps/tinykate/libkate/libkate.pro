TEMPLATE	= lib
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin
HEADERS		= microkde/kapplication.h \
		  microkde/kconfig.h \
                  microkde/kdebug.h \
                  microkde/kdialog.h \
                  microkde/kdialogbase.h \
                  microkde/kfiledialog.h \
                  microkde/kglobal.h \
                  microkde/kiconloader.h \
                  microkde/klineedit.h \
                  microkde/klocale.h \
                  microkde/kmessagebox.h \
                  microkde/kprinter.h \
                  microkde/krestrictedline.h \
                  microkde/kseparator.h \
                  microkde/ksimpleconfig.h \
                  microkde/kstandarddirs.h \
                  microkde/ktempfile.h \
                  microkde/kunload.h \
                  microkde/kurlrequester.h \
                  microkde/kfontdialog.h \
                  microkde/krun.h \
                  microkde/knumvalidator.h \
                  microkde/kstaticdeleter.h \
                  microkde/klistview.h \
                  microkde/kglobalsettings.h \
		  microkde/kcolorbtn.h \
		  \
	          \
		  qt3back/qregexp3.h \
		  kde/ksharedptr.h \
		  document/katebuffer.h  document/katedialogs.h \
		  document/katetextline.h \
		  document/katecmd.h \
		  document/katehighlight.h \
		  document/katecmds.h    document/katedocument.h  \
		  document/katesyntaxdocument.h \
		  view/kateundohistory.h  \
		  view/kateview.h  \
		  view/kateviewdialog.h \
		  interfaces/view.h \
		  interfaces/document.h \
		  ktexteditor/ktexteditor.h

SOURCES		= microkde/kapplication.cpp \
                  microkde/kdialogbase.cpp \
		  microkde/kconfig.cpp \
                  microkde/klocale.cpp \
                  microkde/kmessagebox.cpp \
                  microkde/kprocess.cpp \
                  microkde/kstandarddirs.cpp \
                  microkde/ktempfile.cpp \
                  microkde/kurlrequester.cpp \
                  microkde/kcolordialog.cpp \
                  microkde/kfontdialog.cpp \
                  microkde/krun.cpp \
                  microkde/knumvalidator.cpp \
                  microkde/kglobal.cpp \
                  microkde/kglobalsettings.cpp \
		  microkde/kcolorbtn.cpp \
		  \
		  \
		  qt3back/qregexp3.cpp \
		  ktexteditor/ktexteditor.cpp \
		  document/katebuffer.cpp  document/katedialogs.cpp \
		  document/katehighlight.cpp  \
		  document/katecmd.cpp   \
		  document/katesyntaxdocument.cpp document/katecmds.cpp \
		  document/katedocument.cpp    document/katetextline.cpp \
		  view/kateundohistory.cpp \
		  view/kateview.cpp \
		  view/kateviewdialog.cpp \
		  interfaces/interfaces.cpp

INTERFACES	= 
INCLUDEPATH	+= $(OPIEDIR)/include $(OPIEDIR)/noncore/apps/tinykate/libkate/microkde \
			$(OPIEDIR)/noncore/apps/tinykate/libkate/document \
			$(OPIEDIR)/noncore/apps/tinykate/libkate/view \
			$(OPIEDIR)/noncore/apps/tinykate/libkate/interfaces \
			$(OPIEDIR)/noncore/apps/tinykate/libkate/ktexteditor \
			$(OPIEDIR)/noncore/apps/tinykate/libkate/qt3back
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET		= tinykate

INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(QTDIR)/lib$(PROJMAK)

