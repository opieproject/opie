TEMPLATE	= lib
CONFIG		+= qt warn_on
HEADERS		= fontfactoryttf_qws.h freetypefactoryimpl.h
SOURCES		= fontfactoryttf_qws.cpp freetypefactoryimpl.cpp
qt2:HEADERS	+= qfontdata_p.h
TARGET		= freetypefactory
DESTDIR		= $(OPIEDIR)/plugins/fontfactories
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lfreetype -lopiecore2
VERSION		= 1.0.1

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBFREETYPE2_INC_DIR ) {
    INCLUDEPATH = $$LIBFREETYPE2_INC_DIR $$INCLUDEPATH
}
