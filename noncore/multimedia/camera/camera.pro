MOC_DIR         = ./moc
OBJECTS_DIR     = ./obj
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = zcameraio.h \
                  previewwidget.h \
                  mainwindow.h

SOURCES         = zcameraio.cpp \
                  previewwidget.cpp \
                  mainwindow.cpp \
                  main.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
INTERFACES      =
TARGET          = opiecam

include ( $(OPIEDIR)/include.pro )

