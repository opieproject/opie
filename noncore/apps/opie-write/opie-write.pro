
CONFIG        += qt warn on  quick-app


HEADERS    =    qcleanuphandler.h \
        qcomplextext_p.h \
        qrichtext_p.h \
        qstylesheet.h \
        qtextedit.h \
        mainwindow.h

SOURCES    =    qcomplextext.cpp \
        qstylesheet.cpp \
        qrichtext_p.cpp \
        qrichtext.cpp \
        qtextedit.cpp \
        main.cpp \
        mainwindow.cpp

INCLUDEPATH    += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2

TARGET        = opie-write


include ( $(OPIEDIR)/include.pro )
