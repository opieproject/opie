CONFIG        += qt warn_on debug quick-app

HEADERS         = defines.h \
               logindialog.h \
               settings.h \
               editaccounts.h \
               mailwrapper.h \
               composemail.h \
               accountview.h \
               mainwindow.h \
               viewmail.h \
               viewmailbase.h \
               opiemail.h \
               imapwrapper.h \
               mailtypes.h \
               mailistviewitem.h \
               pop3wrapper.h \
               abstractmail.h  \
               settingsdialog.h \
               statuswidget.h \
               smtpwrapper.h \
               genericwrapper.h \
               mboxwrapper.h \
               sendmailprogress.h \
               newmaildir.h
               
SOURCES         = main.cpp \
               opiemail.cpp \
               mainwindow.cpp \
               accountview.cpp \
               composemail.cpp \
               mailwrapper.cpp \
               imapwrapper.cpp \
               addresspicker.cpp \
               editaccounts.cpp \
               logindialog.cpp \
               viewmail.cpp \
               viewmailbase.cpp \
               settings.cpp \
               mailtypes.cpp \
               pop3wrapper.cpp \
               abstractmail.cpp \
               settingsdialog.cpp \
               statuswidget.cpp \
               smtpwrapper.cpp \
               genericwrapper.cpp \
               mboxwrapper.cpp \
               sendmailprogress.cpp \
               newmaildir.cpp
               
INTERFACES     = editaccountsui.ui \
               selectmailtypeui.ui \
               imapconfigui.ui \
               pop3configui.ui \
               nntpconfigui.ui \
               smtpconfigui.ui \
               addresspickerui.ui \
               logindialogui.ui \
               composemailui.ui \
               settingsdialogui.ui \
               statuswidgetui.ui \
               sendmailprogressui.ui \
               newmaildirui.ui

INCLUDEPATH += $(OPIEDIR)/include

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
    LIBS        += -lqpe -letpan -lssl -lcrypto -lopie -liconv
}else{
    LIBS        += -lqpe -letpan -lssl -lcrypto -lopie
}

TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
