CONFIG += qt warn_on quick-app


TARGET = zsame

HEADERS = StoneField.h StoneWidget.h ZSameWidget.h dropin/krandomsequence.h
SOURCES = StoneField.cpp StoneWidget.cpp ZSameWidget.cpp dropin/krandomsequence.cpp


INCLUDEPATH += $(OPIEDIR)/include dropin
DEPENDPATH  += $(OPIEDIR)/include


# we now also include opie
LIBS += -lqpe -lopiecore2

include ( $(OPIEDIR)/include.pro )
