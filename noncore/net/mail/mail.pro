CONFIG	    += qt warn_on debug quick-app

HEADERS	     = defines.h \
               logindialog.h \
               settings.h \
               editaccounts.h \
               mailwrapper.h \
               composemail.h \
	           accountview.h \
	           mainwindow.h \
	           opiemail.h 
               
SOURCES	     = main.cpp \
	           opiemail.cpp \
	           mainwindow.cpp \
	           accountview.cpp \
               composemail.cpp \
               mailwrapper.cpp \
               imapwrapper.cpp \
               addresspicker.cpp \
               editaccounts.cpp \
               logindialog.cpp \
               settings.cpp 
               
INTERFACES	 = editaccountsui.ui \
               selectmailtypeui.ui \
               imapconfigui.ui \
               pop3configui.ui \
               nntpconfigui.ui \
               smtpconfigui.ui \
               addresspickerui.ui \
               logindialogui.ui \
               composemailui.ui

INCLUDEPATH += $(OPIEDIR)/include
INCLUDEPATH += $(OPIEDIR)/include/libetpan
LIBS	    += -lqpe -lopie -letpan -lssl -lcrypto -ldb
TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
