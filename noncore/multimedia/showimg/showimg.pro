CONFIG          += qt warn_on release quick-app
HEADERS   = showimg.h ImageFileSelector.h settingsdialog.h settingsdialogbase.h
SOURCES   = main.cpp showimg.cpp ImageFileSelector.cpp settingsdialog.cpp settingsdialogbase.cpp
TARGET          = showimg
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -l opieui2

include ( $(OPIEDIR)/include.pro )
