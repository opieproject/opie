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

TRANSLATIONS = ../../../i18n/de/sysinfo.ts \
	 ../../../i18n/en/sysinfo.ts \
	 ../../../i18n/es/sysinfo.ts \
	 ../../../i18n/fr/sysinfo.ts \
	 ../../../i18n/hu/sysinfo.ts \
	 ../../../i18n/ja/sysinfo.ts \
	 ../../../i18n/ko/sysinfo.ts \
	 ../../../i18n/no/sysinfo.ts \
	 ../../../i18n/pl/sysinfo.ts \
	 ../../../i18n/pt/sysinfo.ts \
	 ../../../i18n/pt_BR/sysinfo.ts \
	 ../../../i18n/sl/sysinfo.ts \
	 ../../../i18n/zh_CN/sysinfo.ts \
	 ../../../i18n/zh_TW/sysinfo.ts
