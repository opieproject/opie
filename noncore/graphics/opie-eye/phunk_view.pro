CONFIG += qt warn_on quick-app
DESTDIR = $(OPIEDIR)/bin
TEMPLATE = app
TARGET = opie-eye
# the name of the resulting object


include( gui/gui.pro        )
include( lib/lib.pro        )
include( iface/iface.pro    )
include( impl/doc/doc.pro   )
include( impl/dir/dir.pro   )
include( impl/dcim/dcim.pro )


INCLUDEPATH += . $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include



LIBS += -lqpe -lopiecore2 -lopieui2 -lopiemm2

include( $(OPIEDIR)/include.pro )
