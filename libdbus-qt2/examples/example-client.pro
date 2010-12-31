TEMPLATE = app

CONFIG += qt thread

SOURCES += example-client.cpp
HEADERS += example-client.h

TARGET = example-client

BUILDDIR=$$(BUILDDIR)
isEmpty( BUILDDIR ) {
    error ( Please set BUILDDIR to point to your build directory )
}

INCLUDEPATH += ../

LIBS += -L$$(BUILDDIR)/.libs -L$$(BUILDDIR)  -ldbus-1-qt3 

# use pkg-config to query for the main DBus lib
LIBS += $$system(pkg-config --libs dbus-1)

MOC_DIR = mocdir
OBJECTS_DIR = objdir
