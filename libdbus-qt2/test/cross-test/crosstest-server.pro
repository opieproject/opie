TEMPLATE = app

CONFIG += qt thread

SOURCES += crosstest-server.cpp interfaceimpls.cpp \
           singletestsinterface.cpp testclientproxy.cpp \
           testsinterface.cpp testsignalsinterface.cpp

HEADERS += crosstest-server.h interfaceimpls.h \
           singletestsinterface.h testclientproxy.h \
           testsinterface.h testsignalsinterface.h

TARGET = crosstest-server

# use pkg-config to query for the main DBus lib
QMAKE_CXXFLAGS += $$system(pkg-config --cflags dbus-1-qt3)
LIBS += $$system(pkg-config --libs dbus-1-qt3)

MOC_DIR = mocdir
OBJECTS_DIR = objdir
