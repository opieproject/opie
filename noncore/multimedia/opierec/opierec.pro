#CONFIG   = qt qtopia warn_on  pdaudio
CONFIG   = qt warn_on  opie
#CONFIG    = qt warn_on  quick-app
HEADERS   = adpcm.h \
	pixmaps.h \
	helpwindow.h \
	qtrec.h \
	device.h \
	wavFile.h \
	waveform.h
SOURCES   = adpcm.c \
	helpwindow.cpp \
	main.cpp \
	qtrec.cpp \
	device.cpp \
	wavFile.cpp \
	waveform.cpp
INTERFACES  =

contains(CONFIG, pdaudio) {
#  LIBS += -L/opt/buildroot-opie/output/staging/target/lib -lqpe -lpthread -ljpeg -lpng -lz
  LIBS += -L$(QPEDIR)/lib -lqpe -lpthread -ljpeg -lpng -lz -lopiecore2
  INCLUDEPATH += $(QPEDIR)/include
  DEPENDPATH  += $(QPEDIR)/include
  DEFINES += PDAUDIO
  DEFINES += THREADED
  TARGET    = qperec

#  DESTDIR=$(QPEDIR)/bin
}

contains(CONFIG, opie) {
  INCLUDEPATH += $(OPIEDIR)/include
  DEPENDPATH  += $(OPIEDIR)/include
  DESTDIR=$(OPIEDIR)/bin
  LIBS            += -lqpe -lopiecore2 -lpthread
  TARGET    = opierec
  include ( $(OPIEDIR)/include.pro )
}


