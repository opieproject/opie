TEMPLATE        = lib
CONFIG        += qt warn_on

HEADERS         = mailwrapper.h \
               imapwrapper.h \
               mailtypes.h \
               pop3wrapper.h \
               abstractmail.h  \
               smtpwrapper.h \
               genericwrapper.h \
               mboxwrapper.h \
               settings.h \
               logindialog.h \
               sendmailprogress.h \
               statusmail.h \
               mhwrapper.h \
               nntpwrapper.h \
               generatemail.h \
               storemail.h

SOURCES         = imapwrapper.cpp \
               mailwrapper.cpp \
               mailtypes.cpp \
               pop3wrapper.cpp \
               abstractmail.cpp \
               smtpwrapper.cpp \
               genericwrapper.cpp \
               mboxwrapper.cpp \
               settings.cpp \
               logindialog.cpp \
               sendmailprogress.cpp \
               statusmail.cpp \
               mhwrapper.cpp \
              nntpwrapper.cpp \
              generatemail.cpp \
              storemail.cpp

INTERFACES     = logindialogui.ui \
              sendmailprogressui.ui

INCLUDEPATH += $(OPIEDIR)/include
LIBS        += -lqpe -letpan -lssl -lcrypto

contains( $(CONFIG_TARGET_MACOSX), y ){
    LIBS    += -liconv
}

DESTDIR      = $(OPIEDIR)/lib
TARGET       = mailwrapper

include ( $(OPIEDIR)/include.pro )

!isEmpty( LIBETPAN_INC_DIR ) {
    INCLUDEPATH = $$LIBETPAN_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBETPAN_LIB_DIR ) {
    LIBS = -L$$LIBETPAN_LIB_DIR $$LIBS
}
