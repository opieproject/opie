TEMPLATE	= 	app
CONFIG		= 	qt warn_on debug
#CONFIG		= 	qt warn_on release
HEADERS		= 	accounteditor.h \
		  	addresspicker.h \
		  	composer.h \
		  	composerbase.h \
		  	configdiag.h \
		  	folderwidget.h \
		  	listviewplus.h \
		  	mailtable.h \
		  	mainwindow.h \
		  	mainwindowbase.h \
		  	opendiag.h \
		  	rename.h \
		  	searchdiag.h \
		  	viewmail.h \
		  	viewmailbase.h
SOURCES		= 	accounteditor.cpp \
		  	addresspicker.cpp \
		  	composer.cpp \
		  	composerbase.cpp \
		  	configdiag.cpp \
		  	folderwidget.cpp \
		  	listviewplus.cpp \
		  	mailtable.cpp \
		  	main.cpp \
		  	mainwindow.cpp \
		  	mainwindowbase.cpp \
		  	opendiag.cpp \
		  	rename.cpp \
		  	searchdiag.cpp \
		  	viewmail.cpp \
		  	viewmailbase.cpp
INTERFACES	= 	accounteditorbase.ui \
		  	addresspickerbase.ui \
		  	configdiagbase.ui \
		  	opendiagbase.ui \
		  	renamebase.ui \
		  	searchdiagbase.ui
INCLUDEPATH    += 	$(OPIEDIR)/include libmail
LIBS           += 	-lmail -lqpe -lopie
TARGET		= 	mail 
DESTDIR		= 	$(OPIEDIR)/bin

TRANSLATIONS = ../../i18n/de/mail.ts \
	 ../../i18n/en/mail.ts \
	 ../../i18n/es/mail.ts \
	 ../../i18n/fr/mail.ts \
	 ../../i18n/hu/mail.ts \
	 ../../i18n/ja/mail.ts \
	 ../../i18n/ko/mail.ts \
	 ../../i18n/no/mail.ts \
	 ../../i18n/pl/mail.ts \
	 ../../i18n/pt/mail.ts \
	 ../../i18n/pt_BR/mail.ts \
	 ../../i18n/sl/mail.ts \
	 ../../i18n/zh_CN/mail.ts \
	 ../../i18n/zh_TW/mail.ts
