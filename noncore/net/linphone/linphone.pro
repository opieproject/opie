CONFIG		+= qt warn on  quick-app

HEADERS		= qlinphone.h mainwindow.h linphoneconfig.h
SOURCES		= qlinphone.cpp mainwindow.cpp linphoneconfig.cpp main.cpp
INTERFACES = mainwidget.ui settingsdialog.ui

INCLUDEPATH	+= $(OPIEDIR)/include  /usr/include/linphone  /usr/include/osipua /usr/include/ortp /usr/include/glib-2.0 /usr/lib/glib-2.0/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopie -llinphone
TARGET		= olinphone

include ( $(OPIEDIR)/include.pro )
