DEPENDPATH += $(OPIEDIR)/ioclude
LIBS += -lqpe -lopie
INTERFACES =
include ( $(OPIEDIR)/include.pro )
TEMPLATE = app
CONFIG += debug warn_on qt
DESTDIR = $(OPIEDIR)/bin
TARGET = osearch
INCLUDEPATH = $(OPIEDIR)/include
SOURCES += main.cpp \
           mainwindow.cpp \
           olistview.cpp \
           olistviewitem.cpp \
           adresssearch.cpp \
           todosearch.cpp \
           datebooksearch.cpp \
           searchgroup.cpp \
           resultitem.cpp \
           todoitem.cpp \
           contactitem.cpp \
           eventitem.cpp \
           applnksearch.cpp \
           applnkitem.cpp \
           doclnkitem.cpp \
           doclnksearch.cpp 
HEADERS += mainwindow.h \
           olistview.h \
           olistviewitem.h \
           adresssearch.h \
           todosearch.h \
           datebooksearch.h \
           searchgroup.h \
           resultitem.h \
           todoitem.h \
           contactitem.h \
           eventitem.h \
           applnksearch.h \
           applnkitem.h \
           doclnkitem.h \
           doclnksearch.h 
