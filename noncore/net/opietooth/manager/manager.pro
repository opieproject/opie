TEMPLATE        = app
CONFIG          = qt warn_on debug
#CONFIG          = qt warn_on release
HEADERS         = bluebase.h scandialog.h btlistitem.h
SOURCES         = main.cpp bluebase.cpp scandialog.cpp btlistitem.cpp
INCLUDEPATH     += $(OPIEDIR)/include
INCLUDEPATH     += $(OPIEDIR)/noncore/net/opietooth/lib
DEPENDPATH      += $(OPIEDIR)/include
LIBS            += -lqpe -lopietooth -lopie
INTERFACES      = bluetoothbase.ui  devicedialog.ui  
DESTDIR         = $(OPIEDIR)/bin
TARGET          = bluetooth-manager



TRANSLATIONS = ../../../../i18n/de/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/en/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/es/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/fr/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/hu/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/ja/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/ko/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/no/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pl/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pt/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pt_BR/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/sl/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/zh_CN/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/zh_TW/bluetooth-manager.ts

