TEMPLATE    = lib
CONFIG     += qt warn_on
DESTDIR     = $(OPIEDIR)/lib
HEADERS     = osqlbackend.h    \
              osqldriver.h     \
              osqlerror.h      \
              osqlmanager.h    \
              osqlquery.h      \
              osqlresult.h     \
              osqltable.h      \
              osqlbackendmanager.h \
              osqlitedriver.h
SOURCES     = osqlbackend.cpp   \
              osqldriver.cpp   \
              osqlerror.cpp    \
              osqlmanager.cpp  \
              osqlquery.cpp    \
              osqlresult.cpp   \
              osqltable.cpp    \
              osqlbackendmanager.cpp \
              osqlitedriver.cpp
INTERFACES  =
TARGET      = opiedb2
VERSION     = 1.9.0
INCLUDEPATH = $(OPIEDIR)/include
DEPENDPATH  = $(OPIEDIR)/include
LIBS       += -lopiecore2 -lqpe -lsqlite

!contains( platform, x11 ) {
  include ( $(OPIEDIR)/include.pro )
}

contains( platform, x11 ) {
  LIBS        += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
}

!isEmpty( LIBSQLITE_INC_DIR ) {
    INCLUDEPATH = $$LIBSQLITE_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBSQLITE_LIB_DIR ) {
    LIBS = -L$$LIBSQLITE_LIB_DIR $$LIBS
}
