CONFIG    += qt warn_on release quick-app
HEADERS = textedit.h filePermissions.h
SOURCES = main.cpp textedit.cpp filePermissions.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lopie
TARGET    = textedit

include ( $(OPIEDIR)/include.pro )
