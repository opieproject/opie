INCLUDES=-I$(srcdir) $(DBUS_INCS) $(QT_INCLUDES) \
	     -DDBUS_COMPILATION

dbusincludedir=$(includedir)/dbus-1.0/qt3/dbus

lib_LTLIBRARIES=libdbus-1-qt3.la

dbusinclude_HEADERS=	\
	dbus/qdbuserror.h \
	dbus/qdbusmessage.h \
	dbus/qdbusconnection.h \
	dbus/qdbusvariant.h \
	dbus/qdbusobject.h  \
	dbus/qdbusproxy.h \
	dbus/qdbusmacros.h \
	dbus/qdbusdata.h \
	dbus/qdbusdatalist.h \
	dbus/qdbusdatamap.h \
	dbus/qdbusobjectpath.h \
	dbus/qdbusdataconverter.h

#   don't install qdbusserver.h as long as it is useless (not implemented at all)
#	dbus/qdbusserver.h

libdbus_1_qt3_la_SOURCES =                   \
	dbus/qdbusproxy.h      \
	dbus/qdbusserver.h     \
	qdbusconnection_p.h    \
	qdbusconnection.cpp    \
	qdbuserror.cpp         \
	qdbusintegrator.cpp    \
	qdbusmarshall.cpp      \
	qdbusmessage.cpp       \
	qdbusserver.cpp        \
	qdbusproxy.cpp         \
	qdbusdata.cpp          \
	qdbusdatalist.cpp      \
	qdbusobjectpath.cpp    \
	qdbusdataconverter.cpp

qdbusserver.lo: qdbusserver.moc
qdbusconnection.lo: qdbusconnection.moc
qdbusproxy.lo: qdbusproxy.moc

CLEANFILES = qdbusserver.moc qdbusconnection.moc qdbusproxy.moc

DISTCLEANFILES = dbus-1-qt3.pc

EXTRA_DIST = COPYING \
             COPYING.GPL \
             COPYING.AFL \
             README \
             ChangeLog \
             AUTHORS \
             INSTALL \
             tools/dbusxml2qt3/LICENSE \
             tools/dbusxml2qt3/classgen.h \
             tools/dbusxml2qt3/methodgen.h

libdbus_1_qt3_la_LIBADD= $(LIB_QT) $(DBUS_LIBS)
libdbus_1_qt3_la_LDFLAGS= $(QT_LDFLAGS) -version-info 9:0:9 -no-undefined

# files that are an exception
qdbusserver.moc: dbus/qdbusserver.h
	$(MOC) -o qdbusserver.moc $(srcdir)/dbus/qdbusserver.h

qdbusconnection.moc: qdbusconnection_p.h
	$(MOC) -o qdbusconnection.moc $(srcdir)/qdbusconnection_p.h

qdbusproxy.moc: dbus/qdbusproxy.h
	$(MOC) -o qdbusproxy.moc $(srcdir)/dbus/qdbusproxy.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dbus-1-qt3.pc

# API documentation generation
apidocs:
	cd docs; doxygen doxygen.conf

apidocs-clean:
	cd docs; rm -rf html

# targets required for cross-binding test script
cross-test-compile:
	cd test/cross-test; $(MAKE) cross-test-compile

cross-test-server:
	cd test/cross-test; $(MAKE) cross-test-server

cross-test-client:
	cd test/cross-test; $(MAKE) cross-test-client

# code generator
bin_PROGRAMS = dbusxml2qt3

dbusxml2qt3_SOURCES = tools/dbusxml2qt3/classgen.cpp tools/dbusxml2qt3/main.cpp tools/dbusxml2qt3/methodgen.cpp
dbusxml2qt3_LDADD = $(LIB_QT)
dbusxml2qt3_LDFLAGS= $(QT_LDFLAGS)
