####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kbill
kbill_SOURCES = inputbox.cpp UI.cc Spark.cc Scorelist.cc Picture.cc Network.cc Monster.cc MCursor.cc Library.cc Horde.cc Game.cc Computer.cc Cable.cc Bucket.cc field.cpp kbill.cpp
kbill_LDADD   =  $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET) $(LIB_KIO)


SUBDIRS = docs bitmaps pixmaps 

EXTRA_DIST = kbill.cpp kbill.h kbill.kdelnk kbill.xpm mini-kbill.xpm field.cpp field.h Bucket.cc Bucket.h Cable.cc Cable.h Computer.cc Computer.h Game.cc Game.h Horde.cc Horde.h Library.cc Library.h MCursor.cc MCursor.h  Monster.cc \ Monster.h Network.cc Network.h Picture.cc Picture.h Scorelist.cc Scorelist.h Spark.cc Spark.h \ Strings.h UI.cc objects.h inputbox.cpp inputbox.h

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Games/
	$(INSTALL_DATA) kbill.kdelnk $(kde_appsdir)/Games/kbill.kdelnk
	$(mkinstalldirs) $(kde_icondir)/
	$(INSTALL_DATA) kbill.xpm $(kde_icondir)/kbill.xpm
	$(mkinstalldirs) $(kde_minidir)/
	$(INSTALL_DATA) mini-kbill.xpm $(kde_minidir)/kbill.xpm
	$(mkinstalldirs) $(kde_datadir)/kbill/
	$(INSTALL_DATA) scores $(kde_datadir)/kbill/scores

uninstall-local:
	-rm -f $(kde_appsdir)/Games/kbill.kdelnk
	-rm -f $(kde_icondir)/kbill.xpm
	-rm -f $(kde_minidir)/kbill.xpm
	-rm -f $(kde_datadir)/kbill/scores

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = kbill 

kbill_METASOURCES = USE_AUTOMOC

# the library search path.
kbill_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(kbill_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(KDEDIR)/include/kde.pot `find . -name \*.h -o -name \*.cpp -o -name \*.cc` -o ../po/kbill.pot 

