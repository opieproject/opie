TEMPLATE	= app
CONFIG		= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin

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
	  taskeditoradvanced.h \
	  taskeditoralarms.h

SOURCES	= smalltodo.cpp \
          todomanager.cpp \
	  mainwindow.cpp \
	  main.cpp \
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
	  taskeditoradvanced.cpp \
	  taskeditoralarms.cpp

TARGET		= todolist
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie 

TRANSLATIONS = ../../../i18n/de/todolist.ts \
	 ../../../i18n/da/todolist.ts \
	 ../../../i18n/xx/todolist.ts \
	 ../../../i18n/en/todolist.ts \
	 ../../../i18n/es/todolist.ts \
	 ../../../i18n/fr/todolist.ts \
	 ../../../i18n/hu/todolist.ts \
	 ../../../i18n/ja/todolist.ts \
	 ../../../i18n/ko/todolist.ts \
	 ../../../i18n/no/todolist.ts \
	 ../../../i18n/pl/todolist.ts \
	 ../../../i18n/pt/todolist.ts \
	 ../../../i18n/pt_BR/todolist.ts \
	 ../../../i18n/sl/todolist.ts \
	 ../../../i18n/zh_CN/todolist.ts \
	 ../../../i18n/zh_TW/todolist.ts
