CONFIG += qt release warn_on quick-app
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

TARGET	= osearch

INCLUDEPATH = $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include
LIBS += -lqpe -lopiecore2 -lopiepim2 -lopieui2
include ( $(OPIEDIR)/include.pro )
