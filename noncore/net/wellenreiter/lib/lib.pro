MOC_DIR         = ./tmp
OBJECTS_DIR     = ./tmp
DESTDIR         = $(OPIEDIR)/output/lib
TEMPLATE        = lib
CONFIG          = qt warn_on debug

HEADERS         = $(OPIEDIR)/libopie2/opieui/olistview.h \
                  $(OPIEDIR)/libopie2/opienet/onetutils.h \
                  $(OPIEDIR)/libopie2/opienet/omanufacturerdb.h \
                  $(OPIEDIR)/libopie2/opienet/onetwork.h \
                  $(OPIEDIR)/libopie2/opienet/opcap.h

SOURCES         = $(OPIEDIR)/libopie2/opieui/olistview.cpp \
                  $(OPIEDIR)/libopie2/opienet/onetutils.cpp \
                  $(OPIEDIR)/libopie2/opienet/omanufacturerdb.cpp \
                  $(OPIEDIR)/libopie2/opienet/onetwork.cpp \
                  $(OPIEDIR)/libopie2/opienet/opcap.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib -lpcap
TARGET          = wellenreiter
VERSION         = 1.0.0


