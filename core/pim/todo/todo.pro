#TEMPLATE	= app
CONFIG		= qt warn_on release quick-app
#DESTDIR		= $(OPIEDIR)/bin

HEADERS	= smalltodo.h \
          todomanager.h \
	  mainwindow.h \
	  todoview.h \
	  tableview.h \
	  todotemplatemanager.h \
	  todoeditor.h \
	  todoshow.h \
	  textviewshow.h \
	  templateeditor.h \
	  templatedialog.h \
	  templatedialogimpl.h \
	  quickedit.h \
	  quickeditimpl.h \
	  otaskeditor.h \
	  taskeditoroverview.h \
	  taskeditorstatus.h \
	  taskeditoralarms.h

SOURCES	= smalltodo.cpp \
          todomanager.cpp \
	  mainwindow.cpp \
#	  main.cpp \
	  tableview.cpp \
	  todoview.cpp \
	  todotemplatemanager.cpp \
	  todoeditor.cpp   \
	  todoshow.cpp \
	  textviewshow.cpp \
	  templateeditor.cpp \
	  templatedialog.cpp  \
	  templatedialogimpl.cpp \
	  quickeditimpl.cpp \
	  quickedit.cpp \
	  otaskeditor.cpp \
	  taskeditoroverview.cpp \
	  taskeditorstatus.cpp \
	  taskeditoralarms.cpp

TARGET		= todolist
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lopieui2 -lopiepim2 -lopiedb2

include ( $(OPIEDIR)/include.pro )
