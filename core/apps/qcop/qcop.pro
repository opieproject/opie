TEMPLATE	= app
DESTDIR		= $$(QPEDIR)/bin

CONFIG		+= qtopialib

HEADERS		= qcopimpl.h
SOURCES		= main.cpp qcopimpl.cpp 

target.path     = /bin
INSTALLS        += target
TARGET		= qcop

PACKAGE_DESCRIPTION=Interprocess communication client for the Qtopia environment.
PACKAGE_DEPEND+=qt-embedded (>=$${QTE_VERSION})

TRANSLATIONS=
