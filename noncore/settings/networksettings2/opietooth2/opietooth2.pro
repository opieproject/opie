TEMPLATE        = lib
CONFIG          += qt warn_on release
#CONFIG		+= qt warn_on debug
DESTDIR		= $(OPIEDIR)/lib$(PROJMAK)
HEADERS		= OTDevice.h  \
                  OTDriver.h  \
                  OTGateway.h \
                  OTHCISocket.h \
                  OTInquiry.h \
                  OTDeviceAddress.h \
                  OTIcons.h \
                  OTUUID.h \
                  OTSDPAttribute.h \
                  OTSDPService.h \
                  OTPeer.h \
                  Opietooth.h 
SOURCES		= OTDevice.cpp \
                  OTDriver.cpp \
                  OTDriverList.cpp \
                  OTHCISocket.cpp \
                  OTInquiry.cpp \
                  OTDeviceAddress.cpp \
                  OTUUID.cpp \
                  OTSDPAttribute.cpp \
                  OTSDPService.cpp \
                  OTIcons.cpp \
                  OTPeer.cpp \
                  OTGateway.cpp \
                  Opietooth.cpp
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2 -lbluetooth
INTERFACES	= OTMainGUI.ui OTScanGUI.ui OTManageGUI.ui OTPairingGUI.ui
TARGET		= opietooth2
VERSION		= 1.0.0

include ( $(OPIEDIR)/include.pro )
