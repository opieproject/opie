DESTDIR         = $(OPIEDIR)/lib
TEMPLATE        = lib
CONFIG          += qt warn_on 

HEADERS         = avi.h \
                  imageio.h \
                  zcameraio.h \

SOURCES         = avi.c \
                  imageio.cpp \
                  zcameraio.cpp \

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2
INTERFACES      =
VERSION         = 1.0.1
TARGET          = camera


include ( $(OPIEDIR)/include.pro )

