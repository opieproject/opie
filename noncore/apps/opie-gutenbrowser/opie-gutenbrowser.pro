TEMPLATE  = app
CONFIG    += qt warn_on quick-app
HEADERS   = LibraryDialog.h \
          SearchDialog.h \
          SearchResults.h \
          helpme.h \
          multiline_ex.h \
          openetext.h \
          editTitle.h \
          fontDialog.h \
          optionsDialog.h \
          helpwindow.h \
          output.h \
          NetworkDialog.h \
          gutenbrowser.h \
          resource.h
SOURCES   =  LibraryDialog.cpp \
          LibraryDialogData.cpp \
          SearchDialog.cpp \
          SearchResults.cpp \
          browserDialog.cpp \
          helpme.cpp \
          multiline_ex.cpp \
          fontDialog.cpp \
          openetext.cpp \
          openetextdata.cpp \
          editTitle.cpp \
          ftpsitedlg.cpp \
          optionsDialog.cpp \
          optionsDialogData.cpp \
          helpwindow.cpp \
          output.cpp \
          NetworkDialog.cpp \
          gutenbrowserData.cpp \
          gutenbrowser.cpp \
          main.cpp
TARGET    = gutenbrowser

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
DESTDIR = $(OPIEDIR)/bin
LIBS   += -lqpe -lpthread -lftplib -lopiecore2 -lopieui2

include ( $(OPIEDIR)/include.pro )
