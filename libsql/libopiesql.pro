TEMPLATE        = lib
CONFIG          += qte warn_on release
HEADERS 	= osqlbackend.h \
		  osqldriver.h \
		  osqlerror.h \
		  osqlmanager.h \
		  osqlquery.h \
		  osqlresult.h \
		  osqltable.h \
		  osqlbackendmanager.h \
		  osqlitedriver.h
SOURCES		= osqlbackend.cpp \
		  osqldriver.cpp \
		  osqlerror.cpp \
		  osqlmanager.cpp \
		  osqlquery.cpp \
		  osqlresult.cpp \
		  osqltable.cpp \
		  osqlbackendmanager.cpp \
		  osqlitedriver.cpp

TARGET		= opiesql
INCLUDEPATH += $(OPIEDIR)/include
DESTDIR      = $(OPIEDIR)/lib$(PROJMAK)
LIBS += -lsqlite
#VERSION = 1.0.0




include ( $(OPIEDIR)/include.pro )
