CONFIG        += qt warn_on debug quick-app

HEADERS         = defines.h \
               editaccounts.h \
               composemail.h \
               accountview.h \
               mainwindow.h \
               viewmail.h \
               viewmailbase.h \
               opiemail.h \
               mailistviewitem.h \
               settingsdialog.h \
               statuswidget.h \
               newmaildir.h
               
SOURCES         = main.cpp \
               opiemail.cpp \
               mainwindow.cpp \
               accountview.cpp \
               composemail.cpp \
               addresspicker.cpp \
               editaccounts.cpp \
               viewmail.cpp \
               viewmailbase.cpp \
               settingsdialog.cpp \
               statuswidget.cpp \
               newmaildir.cpp
               
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
               newmaildirui.ui

INCLUDEPATH += $(OPIEDIR)/include

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
    LIBS        += -lqpe -letpan -lssl -lcrypto -lopie -liconv
}else{
    LIBS        += -lqpe -lopie -llibmailwrapper
}

TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
