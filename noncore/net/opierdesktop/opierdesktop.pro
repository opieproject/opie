TEMPLATE        = app
DESTDIR         = $(OPIEDIR)/bin
CONFIG          = qt warn_on debug

HEADERS += constants.h orders.h parse.h proto.h qtwin.h rdesktop.h types.h
SOURCES += bitmap.cpp \
           cache.cpp \
           iso.cpp \
           licence.cpp \
           mcs.cpp \
           orders.cpp \
           qtwin.cpp \
           rdesktop.cpp \
           rdp.cpp \
           secure.cpp \
           tcp.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lm -lcrypto
INTERFACES      =
TARGET          = ordesktop
QMAKE_CXXFLAGS    += -DSHARP=1 -DWITH_OPENSSL=1

include ( $(OPIEDIR)/include.pro )

