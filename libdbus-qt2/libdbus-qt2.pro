TEMPLATE    = lib
#CONFIG      += qt warn_on release
CONFIG      += qt warn_on debug
HEADERS     = dbus/qdbusproxy.h \
              dbus/qdbusserver.h \
              qdbusconnection_p.h \
              qdestroyedsignalconnector.h

SOURCES     = qdbusconnection.cpp \
              qdbuserror.cpp \
              qdbusintegrator.cpp \
              qdbusmarshall.cpp \
              qdbusmessage.cpp \
              qdbusserver.cpp \
              qdbusproxy.cpp \
              qdbusdata.cpp \
              qdbusdatalist.cpp \
              qdbusobjectpath.cpp \
              qdbusdataconverter.cpp

INCLUDEPATH += $$system(pkg-config --cflags-only-I dbus-1 | sed 's/-I//g')
LIBS        += $$system(pkg-config --libs dbus-1)
TARGET      = dbus-qt2
VERSION     = 1.0
