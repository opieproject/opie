DESTDIR         = $(OPIEDIR)/output/lib
TEMPLATE        = lib
CONFIG          = qt warn_on 

HEADERS         = ../libopie2/opiecore/odebug.h \
                  ../libopie2/opieui/olistview.h \
                  ../libopie2/opienet/onetutils.h \
                  ../libopie2/opienet/omanufacturerdb.h \
                  ../libopie2/opienet/odebugmapper.h \
                  ../libopie2/opienet/onetwork.h \
                  ../libopie2/opienet/opcap.h \
                  ../libopie2/opienet/ostation.h

SOURCES         = ../libopie2/opiecore/odebug.cpp \
                  ../libopie2/opieui/olistview.cpp \
                  ../libopie2/opienet/onetutils.cpp \
                  ../libopie2/opienet/omanufacturerdb.cpp \
                  ../libopie2/opienet/odebugmapper.cpp \
                  ../libopie2/opienet/onetwork.cpp \
                  ../libopie2/opienet/opcap.cpp \
                  ../libopie2/opienet/ostation.h

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib -lpcap
INTERFACES      =
TARGET          = wellenreiter
VERSION         = 1.0.2
