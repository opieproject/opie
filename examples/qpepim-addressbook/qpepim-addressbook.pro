TEMPLATE        = app
CONFIG  = qt warn_on debug

HEADERS = addressbookdumper.h
SOURCES = main.cpp addressbookdumper.cpp
TARGET = abexample
INCLUDEPATH     = $(QPEDIR)/include
LIBS       += -lqpepim -lqpe -lqte
