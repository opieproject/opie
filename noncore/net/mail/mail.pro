CONFIG        += qt warn_on  quick-app

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
               selectsmtp.h \
               nntpgroups.h \
               nntpgroupsdlg.h

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
               selectsmtp.cpp \
               nntpgroups.cpp \
               nntpgroupsdlg.cpp

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
               selectstoreui.ui \
               nntpgroupsui.ui


INCLUDEPATH += $(OPIEDIR)/include /usr/local/include

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
    LIBS        += -lqpe -lopieui2 -lopiecore2 -lopiepim2 -lmailwrapper -liconv -lopiemm2
}else{
    LIBS        += -lqpe -lopieui2 -lopiecore2 -lopiepim2 -lmailwrapper -lopiemm2
}

TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
!isEmpty( LIBETPAN_INC_DIR ) {
    INCLUDEPATH = $$LIBETPAN_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBETPAN_LIB_DIR ) {
    LIBS = -Wl,-rpath-link,$$LIBETPAN_LIB_DIR $$LIBS
}
