CONFIG		+= 	qt warn_on  quick-app
#CONFIG		+= 	qt warn_on 
HEADERS		= 	accounteditor.h \
		  	addresspicker.h \
		  	composer.h \
		  	composerbase.h \
		  	configdiag.h \
		  	folderwidget.h \
		  	listviewplus.h \
			mailstatusbar.h \
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
			mailstatusbar.cpp \
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
LIBS           += 	-lcoremail -lqpe -lopie
TARGET		= 	mail 

include ( $(OPIEDIR)/include.pro )
