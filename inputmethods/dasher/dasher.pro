TEMPLATE	= lib
CONFIG		+= qt warn_on release

HEADERS =   Alphabet.h \
	    DasherModel.h \
	    FrameRate.h \
            AlphabetMap.h \
            DasherNode.h \
	    LanguageModel.h \
	    AlphIO.h \
	    DasherScreen.h             \
	    PPMLanguageModel.h \
	    Context.h           \  
	    DasherSettingsInterface.h  \
	    QtDasherImpl.h \
	    CustomAlphabet.h \      
	    DasherTypes.h     \         
	    QtDasherPlugin.h \
	    DashEdit.h       \    
	    DasherView.h      \         
	    QtDasherScreen.h \
	    DasherAppInterface.h \ 
	    DasherViewSquare.h   \      
	    SettingsStore.h \
	    DasherInterface.h \    
	    DasherWidgetInterface.h
SOURCES =   Alphabet.cpp         \
	    DasherModel.cpp       \       
	    LanguageModel.cpp \
	    AlphabetMap.cpp   \   
	    DasherNode.cpp     \          
	    PPMLanguageModel.cpp \
	    AlphIO.cpp           \
	    DasherSettingsInterface.cpp \ 
	    SettingsStore.cpp \
	    CustomAlphabet.cpp \  
	    DasherView.cpp \
	    DasherInterface.cpp \ 
	    DasherViewSquare.cpp \
	    QtDasherImpl.cc \
	    QtDasherPlugin.cc \
	    QtDasherScreen.cc
TARGET		= qdasher
DESTDIR		= ../../plugins/inputmethods
INCLUDEPATH += $(OPIEDIR)/include
DEPENDPATH      += ../$(OPIEDIR)/include ../../launcher
LIBS            += -lqpe 
QMAKE_LFLAGS	+= -Wl,-rpath,/opt/QtPalmtop/plugins/inputmethods
VERSION		= 1.0.0
SUBDIRS		= DasherCore


include ( $(OPIEDIR)/include.pro )
target.path = $$prefix/plugins/inputmethods



