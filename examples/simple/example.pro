# '#' introduces a comment
#CONFIG says the qmake buildsystem what to use
#
CONFIG += qt warn_on quick-app
# qt = Use qt
# warn_on = warnings are on
# quick-app = Tell the buildsystem that the apps is quick-launchable
#

# Note if you do not supply quick-app you need to set
# TEMPLATE = app or TEMPLATE = lib depending on if you want to build an executable or lib
# and DESTDIR= $(OPIEDIR)/bin or $(OPIEDIR)/lib

TARGET = simple
# the name of the resulting object

HEADERS = simple.h
# A list header files


SOURCES = simple.cpp
# A list of source files

INTERFACES =
# list of ui files

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include



LIBS += -lqpe 
# libs to link against

#Important include the global include.pro just like below
 

include ( $(OPIEDIR)/include.pro )
