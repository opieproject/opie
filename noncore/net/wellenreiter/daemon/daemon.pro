DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = warn_on debug
#CONFIG         = warn_on release
HEADERS         = source/config.hh source/daemon.hh
SOURCES         = source/daemon.cc
INCLUDEPATH     += ../
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lpcap -lpthread -L$(OPIEDIR)/lib -lwellenreiter
INTERFACES      = 
TARGET          = wellenreiterd
DEFINES         += DEBUG

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}
