MOC_DIR         = ./moc
OBJECTS_DIR     = ./obj
DESTDIR         = $(OPIEDIR)/lib
TEMPLATE        = lib
CONFIG          = qt warn_on debug

HEADERS         = avi.h \
                  imageio.h \
                  zcameraio.h \

SOURCES         = avi.c \
                  imageio.cpp \
                  zcameraio.cpp \

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lopiecore2
INTERFACES      =
VERSION         = 1.0.0
TARGET          = opiecam


include ( $(OPIEDIR)/include.pro )

