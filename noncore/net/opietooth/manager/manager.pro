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
TARGET          = $(OPIEDIR)/bin/bluetooth-manager



TRANSLATIONS = ../../../../i18n/de/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/en/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/es/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/fr/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/hu/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/ja/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/ko/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/no/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pl/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pt/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/pt_BR/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/sl/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/zh_CN/$(OPIEDIR)/bin/bluetooth-manager.ts
TRANSLATIONS += ../../../../i18n/zh_TW/$(OPIEDIR)/bin/bluetooth-manager.ts

