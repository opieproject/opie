CONFIG    = qt warn_on release quick-app
HEADERS   = memory.h \
      graph.h \
      load.h \
      storage.h \
      processinfo.h \
      modulesinfo.h \
      detail.h \
      benchmarkinfo.h \
      versioninfo.h \
      sysinfo.h
SOURCES   = main.cpp \
      memory.cpp \
      graph.cpp \
      load.cpp \
      storage.cpp \
      processinfo.cpp \
      modulesinfo.cpp \
      detail.cpp \
      benchmarkinfo.cpp fft.c \
      versioninfo.cpp \
      sysinfo.cpp

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lopiecore2 -lopieui2

TARGET    = sysinfo

include ( $(OPIEDIR)/include.pro )
