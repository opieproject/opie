TEMPLATE  = app
#CONFIG    = qt warn_on release
CONFIG         = qt warn_on debug
DESTDIR   = $(OPIEDIR)/bin
HEADERS   = playlistselection.h mediaplayerstate.h xinecontrol.h mediadetect.h\
        videowidget.h audiowidget.h playlistwidget.h mediaplayer.h inputDialog.h \
        frame.h lib.h xinevideowidget.h volumecontrol.h\
              alphablend.h yuv2rgb.h
SOURCES   = main.cpp \
        playlistselection.cpp mediaplayerstate.cpp xinecontrol.cpp mediadetect.cpp\
       videowidget.cpp audiowidget.cpp playlistwidget.cpp mediaplayer.cpp inputDialog.cpp \
   frame.cpp lib.cpp nullvideo.c xinevideowidget.cpp volumecontrol.cpp\
         alphablend.c yuv2rgb.c yuv2rgb_arm.c yuv2rgb_arm4l.S
TARGET    = opieplayer2
INCLUDEPATH += $(OPIEDIR)/include 
DEPENDPATH  += $(OPIEDIR)/include 
LIBS            += -lqpe -lpthread -lopie -lxine -lxineutils
MOC_DIR=qpeobj
OBJECTS_DIR=qpeobj

INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include

