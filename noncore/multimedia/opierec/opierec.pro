#CONFIG   = qt warn_on debug
CONFIG    = qt warn_on release quick-app
HEADERS   = adpcm.h pixmaps.h helpwindow.h qtrec.h device.h wavFile.h
SOURCES   = adpcm.c helpwindow.cpp main.cpp qtrec.cpp device.cpp wavFile.cpp
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH  += $(OPIEDIR)/include
LIBS            += -lqpe -lstdc++ -lpthread
INTERFACES  = 
TARGET    = opierec
#TMAKE_CXXFLAGS += -DQT_QWS_VERCEL_IDR -DQWS -fno-exceptions -fno-rtti
include ( $(OPIEDIR)/include.pro )
