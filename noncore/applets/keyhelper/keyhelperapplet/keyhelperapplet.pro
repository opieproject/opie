TEMPLATE	= lib
CONFIG		= qt plugin warn_on
HEADERS		= anylnk/AnyLnk.h \
		  anylnk/AppLnkManager.h \
		  anylnk/AppLnkWrapper.h \
		  anylnk/DocLnkWrapper.h \
		  anylnk/ExecLnk.h \
		  anylnk/LnkWrapper.h \
		  anylnk/MenuLnk.h \
		  anylnk/ProcessInvoker.h \
		  anylnk/QCopLnk.h \
		  anylnk/TextLnk.h \
		  applet/KeyHelper.h \
		  applet/KeyHelperApplet.h \
		  applet/KeyHelperWidget.h \
		  applet/QSafeDataStream.h \
		  config/ExtensionsHandler.h \
		  config/KeycfgErrorHandler.h \
		  config/KeycfgHandler.h \
		  config/KeycfgReader.h \
		  config/MappingsHandler.h \
		  config/ModifiersHandler.h \
		  config/RepeaterHandler.h \
		  extension/ExtensionFactory.h \
		  extension/ExtensionInterface.h \
		  extension/KeyExtensions.h \
		  extension/KeyLauncher.h \
		  extension/MenuLauncher.h \
		  extension/MenuTitle.h \
		  extension/QPopupMenuEx.h \
		  extension/TaskSelector.h \
		  extension/TaskSwitcher.h \
		  misc/ConfigEx.h \
		  misc/KHUtil.h \
		  misc/KeyAction.h \
		  misc/KeyMappings.h \
		  misc/KeyModifiers.h \
		  misc/KeyNames.h \
		  misc/KeyRepeater.h \
		  misc/StringParser.h
SOURCES		= anylnk/AnyLnk.cpp \
		  anylnk/AppLnkManager.cpp \
		  anylnk/AppLnkWrapper.cpp \
		  anylnk/LnkWrapper.cpp \
		  anylnk/ProcessInvoker.cpp \
		  anylnk/QCopLnk.cpp \
		  anylnk/TextLnk.cpp \
		  applet/KeyHelper.cpp \
		  applet/KeyHelperApplet.cpp \
		  applet/KeyHelperWidget.cpp \
		  applet/QSafeDataStream.cpp \
		  config/ExtensionsHandler.cpp \
		  config/KeycfgErrorHandler.cpp \
		  config/KeycfgHandler.cpp \
		  config/KeycfgReader.cpp \
		  config/MappingsHandler.cpp \
		  config/ModifiersHandler.cpp \
		  config/RepeaterHandler.cpp \
		  extension/ExtensionFactory.cpp \
		  extension/KeyExtensions.cpp \
		  extension/KeyLauncher.cpp \
		  extension/MenuLauncher.cpp \
		  extension/MenuTitle.cpp \
		  extension/TaskSelector.cpp \
		  extension/TaskSwitcher.cpp \
		  misc/ConfigEx.cpp \
		  misc/KHUtil.cpp \
		  misc/KeyAction.cpp \
		  misc/KeyMappings.cpp \
		  misc/KeyModifiers.cpp \
		  misc/KeyNames.cpp \
		  misc/KeyRepeater.cpp \
		  misc/StringParser.cpp
DESTDIR			= $(OPIEDIR)/plugins/applets/
INCLUDEPATH		+= $(OPIEDIR)/include ./misc ./config ./applet ./extension ./anylnk
LIBS			+= -lqpe -lopiecore2
TARGET			= keyhelperapplet
VERSION			= 1.2.2

include( $(OPIEDIR)/include.pro )
