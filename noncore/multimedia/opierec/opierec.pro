CONFIG   = qt warn_on release
#CONFIG    = qt warn_on release quick-app
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
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
#LIBS            += -L/opt/buildroot-opie/output/staging/target/lib -lqpe -lpthread -ljpeg -lpng -lz
LIBS            += -lqpe -lpthread
INTERFACES  =
TARGET    = opierec

include ( $(OPIEDIR)/include.pro )

