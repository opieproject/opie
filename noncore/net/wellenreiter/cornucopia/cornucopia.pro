DESTDIR        = $(OPIEDIR)/lib
MOC_DIR        = ./tmp
OBJECTS_DIR    = ./tmp
TEMPLATE       = lib
CONFIG         = qt warn_on debug
VERSION        = 0.1
HEADERS        = olistview.h
SOURCES        = olistview.cpp
INCLUDEPATH    = $(OPIEDIR)/include
DEPENDPATH     = $(OPIEDIR)/include
TARGET         = cornucopia

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

