TEMPLATE	= 	app
CONFIG		= 	qt warn_on release
HEADERS		= 	accounteditor.h \
		  	addresspicker.h \
		  	attachdiag.h \
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
		  	attachdiag.cpp \
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
LIBS           += 	-lmail -lqpe
TARGET		= 	mail 
DESTDIR		= 	$(OPIEDIR)/bin
