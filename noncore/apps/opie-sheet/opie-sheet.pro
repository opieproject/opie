CONFIG        = qt warn_on  quick-app
HEADERS        = mainwindow.h sheet.h cellformat.h finddlg.h numberdlg.h sortdlg.h textdlg.h Excel.h
SOURCES        = main.cpp mainwindow.cpp sheet.cpp cellformat.cpp finddlg.cpp numberdlg.cpp sortdlg.cpp textdlg.cpp Excel.cpp
INCLUDEPATH    += $(OPIEDIR)/include
DEPENDPATH    += $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET        = opie-sheet

include ( $(OPIEDIR)/include.pro )
