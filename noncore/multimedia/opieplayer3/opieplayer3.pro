CONFIG    = qt warn_on  quick-app
HEADERS   = mwindow.h playlist.h ../opieplayer2/lib.h ../opieplayer2/threadutil.h \
		../opieplayer2/alphablend.h ../opieplayer2/yuv2rgb.h ../opieplayer2/om3u.h \
		audiowidget.h ../opieplayer2/xinevideowidget.h \
		videowidget.h
SOURCES   = mwindow.cpp playlist.cpp ../opieplayer2/lib.cpp ../opieplayer2/threadutil.cpp \
		../opieplayer2/nullvideo.c ../opieplayer2/om3u.cpp \
		../opieplayer2/alphablend.c ../opieplayer2/yuv2rgb.c \
		audiowidget.cpp ../opieplayer2/xinevideowidget.cpp \
		videowidget.cpp ../opieplayer2/yuv2rgb_arm2.c ../opieplayer2/yuv2rgb_arm4l.S

TARGET    = opie-mediaplayer3
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS        += -lqpe -lpthread -lopiecore2 -lopieui2 -lqtaux2 -lxine 
MOC_DIR   = qpeobj
OBJECTS_DIR = qpeobj

include( $(OPIEDIR)/include.pro )

!isEmpty( LIBXINE_INC_DIR ) {
    INCLUDEPATH = $$LIBXINE_INC_DIR $$INCLUDEPATH
}
!isEmpty( LIBXINE_LIB_DIR ) {
    LIBS = -L$$LIBXINE_LIB_DIR $$LIBS
}
