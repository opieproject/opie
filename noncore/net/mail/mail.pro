CONFIG	    += qt warn_on debug quick-app

HEADERS	     = defines.h \
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
				mailistviewitem.h
               
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
		viewmail.cpp \
		viewmailbase.cpp \
               settings.cpp \
				mailtypes.cpp 
               
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
LIBS	    += -lqpe -lopie -letpan -lssl -lcrypto -ldb
TARGET       = opiemail

include ( $(OPIEDIR)/include.pro )
