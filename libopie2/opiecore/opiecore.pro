TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oapplication.h            \
              oconfig.h                 \
              ocompletionbase.h         \
              ocompletion.h             \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              osortablevaluelist.h
              
SOURCES     = oapplication.cpp          \
              oconfig.cpp               \
              ocompletionbase.cpp       \
              ocompletion.cpp           \
              odebug.cpp                \
              oglobal.cpp               \
              oglobalsettings.cpp      
              
INTERFACES  =
TARGET      = opiecore2
VERSION     = 1.8.1
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        = -lqpe
MOC_DIR     = moc
OBJECTS_DIR = obj

include ( $(OPIEDIR)/include.pro )

