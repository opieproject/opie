QMAKESPECSDIR=$(OPIEDIR)/mkspecs

CC		= $(shell which ccache) gcc
CXX		= $(shell which ccache) g++

#qmake code
OBJS=project.o property.o main.o makefile.o unixmake2.o unixmake.o msvc_nmake.o \
     borland_bmake.o mingw_make.o msvc_dsp.o msvc_vcproj.o option.o \
     winmakefile.o projectgenerator.o metrowerks_xml.o  pbuilder_pbx.o \
     msvc_objectmodel.o meta.o qtmd5.o

#qt code
QOBJS=qstring.o qtextstream.o qiodevice.o qglobal.o qgdict.o qcstring.o \
      qdatastream.o qgarray.o qbuffer.o qglist.o qptrcollection.o qfile.o \
      qfile_unix.o qregexp.o qgvector.o qgcache.o qbitarray.o qdir.o quuid.o \
      qfileinfo_unix.o qdir_unix.o qfileinfo.o qdatetime.o qstringlist.o qmap.o \
      qconfig.o qunicodetables.o qsettings.o qlocale.o 

#all sources, used for the depend target
DEPEND_SRC=project.cpp property.cpp meta.cpp main.cpp qtmd5.cpp generators/makefile.cpp generators/unix/unixmake2.cpp \
           generators/unix/unixmake.cpp generators/win32/msvc_nmake.cpp generators/win32/borland_bmake.cpp \
	   generators/win32/winmakefile.cpp generators/projectgenerator.cpp generators/mac/metrowerks_xml.cpp \
	   generators/mac/pbuilder_pbx.cpp generators/win32/msvc_objectmodel.cpp \
	   $(OPIEDIR)/qmake/tools/qstring.cpp $(OPIEDIR)/qmake/tools/qtextstream.cpp \
	   $(OPIEDIR)/qmake/tools/qiodevice.cpp $(OPIEDIR)/qmake/tools/qglobal.cpp \
	   $(OPIEDIR)/qmake/tools/qgdict.cpp $(OPIEDIR)/qmake/tools/qcstring.cpp \
	   $(OPIEDIR)/qmake/tools/qdatastream.cpp $(OPIEDIR)/qmake/src/tools/qgarray.cpp \
	   $(OPIEDIR)/qmake/tools/qbuffer.cpp $(OPIEDIR)/qmake/tools/qglist.cpp \
	   $(OPIEDIR)/qmake/tools/qptrcollection.cpp $(OPIEDIR)/qmake/tools/qfile.cpp \
	   $(OPIEDIR)/qmake/tools/qfile_unix.cpp $(OPIEDIR)/qmake/tools/qregexp.cpp \
	   $(OPIEDIR)/qmake/tools/qgvector.cpp $(OPIEDIR)/qmake/tools/qgcache.cpp \
	   $(OPIEDIR)/qmake/tools/qbitarray.cpp $(OPIEDIR)/qmake/tools/qdir.cpp \
	   $(OPIEDIR)/qmake/tools/quuid.cpp $(OPIEDIR)/qmake/tools/qfileinfo_unix.cpp \
	   $(OPIEDIR)/qmake/tools/qdir_unix.cpp $(OPIEDIR)/qmake/tools/qfileinfo.cpp \
	   $(OPIEDIR)/qmake/tools/qdatetime.cpp $(OPIEDIR)/qmake/tools/qstringlist.cpp \
	   $(OPIEDIR)/qmake/tools/qmap.cpp $(OPIEDIR)/qmake/tools/qconfig.cpp \
	   $(OPIEDIR)/qmake/tools/qsettings.cpp $(OPIEDIR)/qmake/tools/qurl.cpp \
	   $(OPIEDIR)/qmake/tools/qsettings_mac.cpp $(OPIEDIR)/qmake/tools/qlocale.cpp

CXXFLAGS=  \
	-I. -Igenerators -Igenerators/unix -Igenerators/win32 -Igenerators/mac \
	-I$(OPIEDIR)/qmake/include/qmake -I$(OPIEDIR)/qmake/include -I$(OPIEDIR)/qmake/include/private \
	-DQT_NO_TEXTCODEC -DQT_NO_UNICODETABLES -DQT_NO_COMPONENT -DQT_NO_STL \
	-DQT_NO_COMPRESS -I$(QMAKESPECSDIR)/default -DHAVE_QCONFIG_CPP
LFLAGS=

qmake: $(OBJS) $(QOBJS)
	$(CXX) -o $@ $(OBJS) $(QOBJS) $(LFLAGS)
	$(if $(OPIEDIR),rm -f $(OPIEDIR)/bin/$@)
	$(if $(OPIEDIR),ln -s ../qmake/$@ $(OPIEDIR)/bin/$@)

install: qmake
	[ -d $(INSTALL_ROOT)$(QTDIR)/bin ] || mkdir -p $(INSTALL_ROOT)$(QTDIR)/bin
	-cp -f $(QTDIR)/bin/qmake $(INSTALL_ROOT)$(QTDIR)/bin
	[ -d $(INSTALL_ROOT)$(QTDIR) ] || mkdir -p $(INSTALL_ROOT)$(QTDIR)
	-cp -r -f $(QMAKESPECSDIR) $(INSTALL_ROOT)$(QTDIR)

clean::
	rm -f $(OBJS) $(QOBJS)

distclean:: clean
	rm -rf qmake .deps


# don't use optimization for these
qtextstream.o: $(OPIEDIR)/qmake/tools/qtextstream.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qtextstream.cpp

qiodevice.o: $(OPIEDIR)/qmake/tools/qiodevice.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qiodevice.cpp

qglobal.o: $(OPIEDIR)/qmake/tools/qglobal.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qglobal.cpp

qgdict.o: $(OPIEDIR)/qmake/tools/qgdict.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qgdict.cpp

qcstring.o: $(OPIEDIR)/qmake/tools/qcstring.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qcstring.cpp

qsettings.o: $(OPIEDIR)/qmake/tools/qsettings.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qsettings.cpp

qsettings_mac.o: $(OPIEDIR)/qmake/tools/qsettings_mac.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qsettings_mac.cpp

qurl.o: $(OPIEDIR)/qmake/kernel/qurl.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/kernel/qurl.cpp

qstring.o: $(OPIEDIR)/qmake/tools/qstring.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qstring.cpp

qlocale.o: $(OPIEDIR)/qmake/tools/qlocale.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qlocale.cpp

qdatastream.o: $(OPIEDIR)/qmake/tools/qdatastream.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qdatastream.cpp

qunicodetables.o: $(OPIEDIR)/qmake/tools/qunicodetables.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qunicodetables.cpp

qgarray.o: $(OPIEDIR)/qmake/tools/qgarray.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qgarray.cpp

qbuffer.o: $(OPIEDIR)/qmake/tools/qbuffer.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qbuffer.cpp

qglist.o: $(OPIEDIR)/qmake/tools/qglist.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qglist.cpp

qptrcollection.o: $(OPIEDIR)/qmake/tools/qptrcollection.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qptrcollection.cpp

qfile.o: $(OPIEDIR)/qmake/tools/qfile.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qfile.cpp

qfile_unix.o: $(OPIEDIR)/qmake/tools/qfile_unix.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qfile_unix.cpp

qregexp.o: $(OPIEDIR)/qmake/tools/qregexp.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qregexp.cpp

qgvector.o: $(OPIEDIR)/qmake/tools/qgvector.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qgvector.cpp

qgcache.o: $(OPIEDIR)/qmake/tools/qgcache.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qgcache.cpp

qbitarray.o: $(OPIEDIR)/qmake/tools/qbitarray.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qbitarray.cpp

qdir.o: $(OPIEDIR)/qmake/tools/qdir.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qdir.cpp

quuid.o: $(OPIEDIR)/qmake/tools/quuid.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/quuid.cpp

qfileinfo_unix.o: $(OPIEDIR)/qmake/tools/qfileinfo_unix.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qfileinfo_unix.cpp

qdir_unix.o: $(OPIEDIR)/qmake/tools/qdir_unix.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qdir_unix.cpp

qfileinfo.o: $(OPIEDIR)/qmake/tools/qfileinfo.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qfileinfo.cpp

qdatetime.o: $(OPIEDIR)/qmake/tools/qdatetime.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qdatetime.cpp

qstringlist.o: $(OPIEDIR)/qmake/tools/qstringlist.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qstringlist.cpp

qmap.o: $(OPIEDIR)/qmake/tools/qmap.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qmap.cpp

qconfig.o: $(OPIEDIR)/qmake/tools/qconfig.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $(OPIEDIR)/qmake/tools/qconfig.cpp

winmakefile.o: generators/win32/winmakefile.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/winmakefile.cpp

project.o: project.cpp project.h option.h
	$(CXX) -c -o $@ $(CXXFLAGS) project.cpp

property.o: property.cpp project.h option.h
	$(CXX) -c -o $@ $(CXXFLAGS) property.cpp

meta.o: meta.cpp project.h option.h
	$(CXX) -c -o $@ $(CXXFLAGS) meta.cpp

main.o: main.cpp project.h
	$(CXX) -c -o $@ $(CXXFLAGS) main.cpp

option.o: option.cpp option.h
	$(CXX) -c -o $@ $(CXXFLAGS) option.cpp

qtmd5.o: qtmd5.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) qtmd5.cpp

makefile.o: generators/makefile.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/makefile.cpp

unixmake.o: generators/unix/unixmake.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/unix/unixmake.cpp

unixmake2.o: generators/unix/unixmake2.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/unix/unixmake2.cpp

borland_bmake.o: generators/win32/borland_bmake.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/borland_bmake.cpp

mingw_make.o: generators/win32/mingw_make.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/mingw_make.cpp

msvc_objectmodel.o: generators/win32/msvc_objectmodel.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/msvc_objectmodel.cpp

msvc_vcproj.o: generators/win32/msvc_vcproj.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/msvc_vcproj.cpp

msvc_nmake.o: generators/win32/msvc_nmake.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/msvc_nmake.cpp

metrowerks_xml.o: generators/mac/metrowerks_xml.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/mac/metrowerks_xml.cpp

pbuilder_pbx.o: generators/mac/pbuilder_pbx.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/mac/pbuilder_pbx.cpp

msvc_dsp.o: generators/win32/msvc_dsp.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/win32/msvc_dsp.cpp

projectgenerator.o: generators/projectgenerator.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) generators/projectgenerator.cpp

#default rules
.cpp.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $<

# DO NOT DELETE THIS LINE -- make depend depends on it
