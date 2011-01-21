TEMPLATE = app

CONFIG += qt thread

SOURCES += example-service.cpp
HEADERS += example-service.h

TARGET = example-service

BUILDDIR=$$(BUILDDIR)
isEmpty( BUILDDIR ) {
    error ( Please set BUILDDIR to point to your build directory )
}

INCLUDEPATH += ../

LIBS += -L$$(BUILDDIR)/.libs -L$$(BUILDDIR)  -ldbus-qt2

# use pkg-config to query for the main DBus lib
LIBS += $$system(pkg-config --libs dbus-1)

MOC_DIR = mocdir
OBJECTS_DIR = objdir
