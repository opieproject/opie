TEMPLATE	= app
#CONFIG		= qt warn_on debug
CONFIG		= qt warn_on release
#HEADERS		=
SOURCES		= test_todo.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie  -lopiesql
TARGET		= todo_test

include ( $(OPIEDIR)/  )