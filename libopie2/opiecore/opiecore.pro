TEMPLATE    = lib
CONFIG      += qt warn_on debug
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = oconfig.h                 \
              ocompletionbase.h         \
              ocompletion.h             \
              odebug.h                  \
              oglobal.h                 \
              oglobalsettings.h         \
              osortablevaluelist.h
              
SOURCES     = oconfig.cpp               \
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
MOC_DIR     = moc
OBJECTS_DIR = obj


!contains( platform, x11 ) {
  HEADERS    += oapplication.h
  SOURCES    += oapplication.cpp
  LIBS        = -lqpe
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  message( NOT building oapplication for X11 )
  LIBS        = -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib 
}

