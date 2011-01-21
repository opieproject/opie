TEMPLATE = app

CONFIG += qt thread

SOURCES += crosstest-client.cpp \
           singletestsproxy.cpp testclientinterface.cpp \
           testsproxy.cpp testsignalsproxy.cpp

HEADERS += crosstest-client.h \
           singletestsproxy.h testclientinterface.h \
           testsproxy.h testsignalsproxy.h

TARGET = crosstest-client

# use pkg-config to query for the main DBus lib
QMAKE_CXXFLAGS += $$system(pkg-config --cflags dbus-qt2)
LIBS += $$system(pkg-config --libs dbus-qt2)

MOC_DIR = mocdir
OBJECTS_DIR = objdir
