MOC_DIR         = ./tmp
OBJECTS_DIR     = ./tmp
DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug

HEADERS         = wellenreiterbase.h \
                  mainwindow.h \
                  wellenreiter.h \
                  scanlist.h \
                  logwindow.h \
                  hexwindow.h \
                  configwindow.h \
                  manufacturers.h

SOURCES         = main.cpp \
                  mainwindow.cpp \
                  wellenreiterbase.cpp \
                  wellenreiter.cpp \
                  scanlist.cpp \
                  logwindow.cpp \
                  hexwindow.cpp \
                  configwindow.cpp \
                  manufacturers.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lopiecore2 -lopieui2 -lopienet2
INTERFACES      = configbase.ui
TARGET          = wellenreiter

!contains( platform, x11 ) {
  message( qws )
  include ( $(OPIEDIR)/include.pro )
  LIBS += -lqpe -lopie
}

contains( platform, x11 ) {
  LIBS += -L$(OPIEDIR)/lib -Wl,-rpath,$(OPIEDIR)/lib
  SOURCES += resource.cpp
  HEADERS += resource.h
}

