CONFIG        += qt warn_on debug quick-app

HEADERS         = defines.h \
               editaccounts.h \
               composemail.h \
               accountview.h \
               accountitem.h \
               mainwindow.h \
               viewmail.h \
               viewmailbase.h \
               opiemail.h \
               mailistviewitem.h \
               settingsdialog.h \
               statuswidget.h \
               newmaildir.h \
               selectstore.h \
               selectsmtp.h
               
SOURCES         = main.cpp \
               opiemail.cpp \
               mainwindow.cpp \
               accountview.cpp \
               accountitem.cpp \
               composemail.cpp \
               addresspicker.cpp \
               editaccounts.cpp \
               viewmail.cpp \
               viewmailbase.cpp \
               mailistviewitem.cpp \
               settingsdialog.cpp \
               statuswidget.cpp \
               newmaildir.cpp \
               selectstore.cpp \
               selectsmtp.cpp
               
INTERFACES     = editaccountsui.ui \
               selectmailtypeui.ui \
               imapconfigui.ui \
               pop3configui.ui \
               nntpconfigui.ui \
               smtpconfigui.ui \
               addresspickerui.ui \
               composemailui.ui \
               settingsdialogui.ui \
               statuswidgetui.ui \
               newmaildirui.ui \
               selectstoreui.ui
	

INCLUDEPATH += $(OPIEDIR)/include

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
    LIBS        += -lqpe -lopie -lmailwrapper -liconv
}else{
    LIBS        += -lqpe -lopie -lmailwrapper
}

TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
