TEMPLATE    = app
CONFIG      = qt warn_on
HEADERS     =
SOURCES     = main.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopiedb2 -lsqlite
TARGET      = sqltest

include ( $(OPIEDIR)/include.pro )

!isEmpty( LIBSQLITE_INC_DIR ) {
    INCLUDEPATH = $$LIBSQLITE_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBSQLITE_LIB_DIR ) {
    LIBS = -L$$LIBSQLITE_LIB_DIR $$LIBS
}
