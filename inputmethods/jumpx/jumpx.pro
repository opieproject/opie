TEMPLATE        = lib
CONFIG         += qt warn_on release
HEADERS         = keyboard.h \
	          keyboardimpl.h
SOURCES         = keyboard.cpp \
                  keyboardimpl.cpp
TARGET          = qjumpx
DESTDIR         = ../../plugins/inputmethods
INCLUDEPATH    += ../../include/qpe
DEPENDPATH     += ../../include/qpe
LIBS           += -lqpe
VERSION         = 1.0.0
