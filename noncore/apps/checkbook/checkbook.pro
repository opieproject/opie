CONFIG    = qt warn_on quick-app
HEADERS   = mainwindow.h \
      cbinfo.h        \
      traninfo.h      \
      graphinfo.h     \
      configuration.h \
      password.h      \
      checkbook.h     \
      transaction.h   \
      tabledef.h      \
      listedit.h      \
      cfg.h           \
      graph.h
SOURCES   = main.cpp    \
      mainwindow.cpp    \
      cbinfo.cpp        \
      traninfo.cpp      \
      graphinfo.cpp     \
      configuration.cpp \
      password.cpp      \
      checkbook.cpp     \
      transaction.cpp   \
      tabledef.cpp      \
      listedit.cpp      \
      cfg.cpp           \
      graph.cpp
INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS    += -lqpe -lopiecore2 -lopieui2
TARGET    = checkbook

include ( $(OPIEDIR)/include.pro )
