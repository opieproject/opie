# make install

# base opie install path
prefix = /opt/QtPalmtop

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

lupdate.target = lupdate
lupdate.commands = lupdate -noobsolete $(PRO)

lrelease.target = lrelease
lrelease.commands = lrelease $(PRO)

ipk.target = ipk
ipk.commands = tmp=`mktemp -d /tmp/ipkg-opie.XXXXXXXXXX` && ( $(MAKE) INSTALL_ROOT="$$$$tmp" install && ipkg-build $$$$tmp; rm -rf $$$$tmp; )

QMAKE_EXTRA_UNIX_TARGETS += lupdate lrelease ipk
QMAKE_LFLAGS += -Wl,-rpath=$$prefix/lib
QMAKE_LIBDIR += $(OPIEDIR)/lib

include ( $(OPIEDIR)/gen.pro )
