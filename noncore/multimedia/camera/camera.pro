MOC_DIR         = ./moc
OBJECTS_DIR     = ./obj
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = avi.h \
                  imageio.h \
                  zcameraio.h \
                  previewwidget.h \
                  mainwindow.h

SOURCES         = avi.c \
                  imageio.cpp \
                  zcameraio.cpp \
                  previewwidget.cpp \
                  mainwindow.cpp \
                  main.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopie -lopiecore2
INTERFACES      =
TARGET          = opiecam

include ( $(OPIEDIR)/include.pro )

