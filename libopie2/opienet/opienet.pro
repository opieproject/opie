TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = 802_11_user.h       \
              dhcp.h              \
              udp_ports.h         \
              wireless.h          \
              odebugmapper.h      \
              omanufacturerdb.h   \
              onetutils.h         \
              onetwork.h          \
              opcap.h             \
              ostation.h
SOURCES     = odebugmapper.cpp       \
              omanufacturerdb.cpp \
              onetutils.cpp       \
              onetwork.cpp        \
              opcap.cpp           \
              ostation.cpp
INTERFACES  =
TARGET      = opienet2
VERSION     = 1.8.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lpcap


!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}
