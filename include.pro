include ( $(OPIEDIR)/gen.pro )

# base opie install path
# prefix = /opt/QtPalmtop ??? Why here (eilers) ???
prefix = $(OPIEDIR)

contains( CONFIG, quick-app-lib ) {
    TEMPLATE = lib
    CONFIG += plugin  # "plugin" will be removed automatically later if not MacOSX ! (eilers)
    DESTDIR  = $(OPIEDIR)/plugins/application
    DEFINES += OPIE_APP_INTERFACE
    !contains( TARGET, launcher ) {
        message( Linking $$TARGET to quicklauncher )
	system( rm -f $$(OPIEDIR)/bin/$$TARGET )
        system( ln -s quicklauncher $$(OPIEDIR)/bin/$$TARGET )
    }
}

contains ( CONFIG, quick-app-bin ) {
    TEMPLATE = app
    DESTDIR  = $(OPIEDIR)/bin
    DEFINES -= OPIE_APP_INTERFACE
    message( Touching plugins/application/lib$${TARGET}.so.0 )
    system( touch $$(OPIEDIR)/plugins/application/lib$${TARGET}.so.0 )
}


contains( TEMPLATE, lib ) {
  target.path = $$prefix/lib
}
!contains( TEMPLATE, lib ) {
  target.path = $$prefix/bin
}
INSTALLS += target

# ipkg control files
control.path = /CONTROL
control.files = control postinst prerm postrm preinst conffiles
INSTALLS += control

# images, default is $$prefix/pics/$$TARGET
pics.path = $$prefix/pics/$$TARGET
pics.files = pics/*
INSTALLS += pics

# sounds, default path is $$prefix/sounds/$$TARGET
sounds.path = $$prefix/sounds/$$TARGET
sounds.files = sounds/*
INSTALLS += sounds

# init scripts, default path is /etc/init.d
init.path = /etc/init.d
init.files = init.d/*
INSTALLS += init

# data, default path is /usr/share/$$TARGET
data.path = /usr/share/$$TARGET
data.files = share/*
INSTALLS += data

etc.path = $$prefix/etc/
etc.files = etc/*
INSTALLS += etc

apps.path = $$prefix/apps/
apps.files = apps/*
INSTALLS += apps

# sounds, default path is $$prefix/sounds/$$TARGET
sounds.path = $$prefix/sounds/$$TARGET
sounds.files = sounds/*
INSTALLS += sounds

# anything in nonstandard paths
root.path = /
root.files = root/*
INSTALLS += root

# new targets
opie-lupdate.target = opie-lupdate
opie-lupdate.commands = opie-lupdate  $(PRO)

opie-lrelease.target = opie-lrelease
opie-lrelease.commands = opie-lrelease $(PRO)

lupdate.target = lupdate
lupdate.commands = lupdate -noobsolete $(PRO)

lrelease.target = lrelease
lrelease.commands = lrelease $(PRO)

# new message target to get all strings from the apps with and without tr
messages.target = messages
messages.commands = xgettext -C -n -ktr -kQT_TRANSLATE_NOOP $$HEADERS $$SOURCES -o '$(OPIEDIR)/messages-$(QMAKE_TARGET)-tr.po' && xgettext -C -n -a $$HEADERS $$SOURCES -o '$(OPIEDIR)/messages-$(QMAKE_TARGET)-allstrings.po'

ipk.target = ipk
ipk.commands = tmp=`mktemp -d /tmp/ipkg-opie.XXXXXXXXXX` && ( $(MAKE) INSTALL_ROOT="$$$$tmp" install && ipkg-build $$$$tmp; rm -rf $$$$tmp; )

QMAKE_EXTRA_UNIX_TARGETS += lupdate lrelease ipk opie-lupdate opie-lrelease messages
CONFTEST = $$system( echo $CONFIG_TARGET_MACOSX )
contains( CONFTEST, y ){
QMAKE_LFLAGS += -Wl
LIBS -= -ldl
LIBS -= -lcrypt 
LIBS -= -lm
}
else {
QMAKE_LFLAGS += -Wl,-rpath=$$prefix/lib
# I am not sure whether it is a good idea to change the way plugins is build
# on linux. Therefore I remove the "plugin" term, which is needed by MacOS-X
CONFIG -= plugin
}
QMAKE_LIBDIR += $(OPIEDIR)/lib

MOC_DIR=.moc/$(PLATFORM)
OBJECTS_DIR=.obj/$(PLATFORM)

#was here now at thetop
#include ( $(OPIEDIR)/gen.pro )
