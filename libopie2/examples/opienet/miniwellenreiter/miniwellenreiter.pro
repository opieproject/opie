TEMPLATE        = app
CONFIG          = qt warn_on debug
HEADERS         =
SOURCES         = miniwellenreiter.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopienet2
TARGET          = miniwellenreiter
MOC_DIR         = moc
OBJECTS_DIR     = obj

include ( $(OPIEDIR)/include.pro )

