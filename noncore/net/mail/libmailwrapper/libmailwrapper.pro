TEMPLATE        = lib
CONFIG        += qt warn_on debug 

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
		sendmailprogress.h
               
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
		sendmailprogress.cpp

INTERFACES 	= logindialogui.ui \
		  sendmailprogressui.ui
               

INCLUDEPATH += $(OPIEDIR)/include

CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
    LIBS        += -lqpe -letpan -lssl -lcrypto -liconv
}else{
    LIBS        += -lqpe -letpan -lssl -lcrypto  
}

DESTDIR         = $(OPIEDIR)/lib$(PROJMAK)
TARGET       = mailwrapper

include ( $(OPIEDIR)/include.pro )
