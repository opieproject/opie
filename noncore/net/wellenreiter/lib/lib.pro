MOC_DIR         = ./tmp
OBJECTS_DIR     = ./tmp
DESTDIR         = $(OPIEDIR)/lib
TEMPLATE        = lib
CONFIG          = qt warn_on debug

HEADERS         = $(OPIEDIR)/libopie2/opieui/olistview.h \
                  $(OPIEDIR)/libopie2/opienet/onetutils.h \
                  $(OPIEDIR)/libopie2/opienet/onetwork.h \
                  $(OPIEDIR)/libopie2/opienet/opcap.h

SOURCES         = $(OPIEDIR)/libopie2/opieui/olistview.cpp \
                  $(OPIEDIR)/libopie2/opienet/onetutils.cpp \
                  $(OPIEDIR)/libopie2/opienet/onetwork.cpp \
                  $(OPIEDIR)/libopie2/opienet/opcap.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            = -lpcap
TARGET          = wellenreiter
VERSION         = 1.0.0

contains( platform, x11 ) {
  LIBS += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
  SOURCES += resource.cpp
  HEADERS += resource.h
}

