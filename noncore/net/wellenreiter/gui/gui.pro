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
                  statwindow.h \
                  configwindow.h \
                  graphwindow.h \
                  protolistview.h

SOURCES         = main.cpp \
                  mainwindow.cpp \
                  wellenreiterbase.cpp \
                  wellenreiter.cpp \
                  scanlist.cpp \
                  logwindow.cpp \
                  hexwindow.cpp \
                  statwindow.cpp \
                  configwindow.cpp \
                  graphwindow.cpp \
                  protolistview.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
INTERFACES      = configbase.ui
TARGET          = wellenreiter

!contains( platform, x11 ) {
  message( qws )
  include ( $(OPIEDIR)/include.pro )
  LIBS += -lqpe -lopie -lopiecore2 -lopieui2 -lopienet2 -lstdc++
}

contains( platform, x11 ) {
  LIBS += -L$(OPIEDIR)/output/lib -Wl,-rpath,$(OPIEDIR)/output/lib -Wl,-rpath,/usr/local/lib -lwellenreiter
  SOURCES += resource.cpp
  HEADERS += resource.h
  DESTDIR = $(OPIEDIR)/output/bin
}

