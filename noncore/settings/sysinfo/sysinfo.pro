TEMPLATE  = app
CONFIG    = qt warn_on release
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = memory.h \
      graph.h \
      load.h \
      storage.h \
      processinfo.h \
      processdetail.h \
      versioninfo.h \
      sysinfo.h
SOURCES   = main.cpp \
      memory.cpp \
      graph.cpp \
      load.cpp \
      storage.cpp \
      processinfo.cpp \
      processdetail.cpp \
      versioninfo.cpp \
      sysinfo.cpp
INTERFACES  = 

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe

TARGET    = sysinfo

TRANSLATIONS = ../i18n/de/sysinfo.ts
TRANSLATIONS += ../i18n/pt/sysinfo.ts
TRANSLATIONS += ../i18n/pt_BR/sysinfo.ts
TRANSLATIONS   += ../i18n/en/sysinfo.ts
TRANSLATIONS   += ../i18n/hu/sysinfo.ts
TRANSLATIONS   += ../i18n/ja/sysinfo.ts
TRANSLATIONS   += ../i18n/ko/sysinfo.ts
TRANSLATIONS   += ../i18n/fr/sysinfo.ts
TRANSLATIONS   += ../i18n/no/sysinfo.ts
TRANSLATIONS   += ../i18n/pl/sysinfo.ts
TRANSLATIONS   += ../i18n/sl/sysinfo.ts
TRANSLATIONS   += ../i18n/zh_CN/sysinfo.ts
TRANSLATIONS   += ../i18n/zh_TW/sysinfo.ts
TRANSLATIONS   += ../i18n/es/sysinfo.ts
