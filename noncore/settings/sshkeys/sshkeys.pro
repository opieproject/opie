TEMPLATE = app
CONFIG += qt warn_on release
HEADERS = sshkeys.h
SOURCES = main.cpp sshkeys.cpp
TARGET = sshkeys
INTERFACES = sshkeysbase.ui
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH += $(OPIEDIR)/include
LIBS += -lqpe -lopie
DESTDIR = $(OPIEDIR)/bin

