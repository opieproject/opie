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
                  manufacturers.h \
                  graphwindow.h

SOURCES         = main.cpp \
                  mainwindow.cpp \
                  wellenreiterbase.cpp \
                  wellenreiter.cpp \
                  scanlist.cpp \
                  logwindow.cpp \
                  hexwindow.cpp \
                  statwindow.cpp \
                  configwindow.cpp \
                  manufacturers.cpp \
                  graphwindow.cpp

INCLUDEPATH     += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
INTERFACES      = configbase.ui #configbasenew.ui
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

TRANSLATIONS  = ../../../../i18n/de/wellenreiter.ts \
                ../../../../i18n/nl/wellenreiter.ts \
                ../../../../i18n/da/wellenreiter.ts \
                ../../../../i18n/xx/wellenreiter.ts \
                ../../../../i18n/en/wellenreiter.ts \
                ../../../../i18n/es/wellenreiter.ts \
                ../../../../i18n/fr/wellenreiter.ts \
                ../../../../i18n/hu/wellenreiter.ts \
                ../../../../i18n/ja/wellenreiter.ts \
                ../../../../i18n/ko/wellenreiter.ts \
                ../../../../i18n/no/wellenreiter.ts \
                ../../../../i18n/pl/wellenreiter.ts \
                ../../../../i18n/pt/wellenreiter.ts \
                ../../../../i18n/pt_BR/wellenreiter.ts \
                ../../../../i18n/sl/wellenreiter.ts \
                ../../../../i18n/zh_CN/wellenreiter.ts \
                ../../../../i18n/zh_TW/wellenreiter.ts

