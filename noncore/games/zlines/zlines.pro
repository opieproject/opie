CONFIG		= qt quick-app
HEADERS		= ballpainter.h cfg.h linesboard.h prompt.h cell.h field.h klines.h mwidget.h
SOURCES		= ballpainter.cpp field.cpp klines.cpp main.cpp prompt.cpp cell.cpp linesboard.cpp mwidget.cpp
TARGET		= zlines
LIBS		+= -lqpe -lopiecore2
INCLUDEPATH	+= $(OPIEDIR)/include/
DEPENDPATH	+= $(OPIEDIR)/include/


include ($(OPIEDIR)/include.pro)
