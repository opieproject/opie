#############################################################################
#!
#! This is a tmake template for building UNIX libraries.
#!
#! Actually, it will be trivial to make app vs. lib a config option.
#!
#${
    Project('CONFIG *= propagate');
    $project{"TMAKE_CXXFLAGS"} = "";
    $project{"TMAKE_CFLAGS"} = "";
    $project{"TMAKE_LFLAGS"} = "";
    $project{"TMAKE_LIBS"} = "";

    Project('TMAKE_LIBS += $$LIBS'); # Misc. project-specific extras

    if ( !Project("INTERFACE_DECL_PATH") ) {
	Project('INTERFACE_DECL_PATH = .' );
    }
    if ( Config("qgl") ) {
	#! qgl apps need X11 includes and libraries
	Project('CONFIG *= x11lib');
	Project('CONFIG *= x11inc');
    }

    if ( Config("x11") ) {
	Project('CONFIG *= x11lib');
	Project('CONFIG *= x11inc');
    }
    if ( Config("opengl") ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_OPENGL)');
	Project('TMAKE_LFLAGS *= $(SYSCONF_LFLAGS_OPENGL)');
	Project('TMAKE_LIBS *= $(SYSCONF_LIBS_OPENGL)');
    }
    if ( Config("qt") ) {
	$moc_aware = 1;
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_QT)');
	if ( Project("TARGET") ne "qt"
	  && Project("TARGET") ne "qte" ) {
	    Project('TMAKE_LFLAGS *= $(SYSCONF_LFLAGS_QT)');
	    Project('TMAKE_LFLAGS *= $(SYSCONF_RPATH_QT)');
	    Project('TMAKE_LIBS *= $(SYSCONF_LIBS_QT)');
	}
    } elsif ( Config("qtinc") ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_QT)');
    }
    if ( Config("x11inc") ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_X11)');
    }
    if ( Config("x11lib") ) {
	Project('TMAKE_LFLAGS *= $(SYSCONF_LFLAGS_X11)');
	Project('TMAKE_LFLAGS *= $(SYSCONF_RPATH_X11)');
	Project('TMAKE_LIBS *= $(SYSCONF_LIBS_X11)');
    }

    ##### These may still need replacing
    if ( !Project("TMAKE_RUN_CC") ) {
	Project('TMAKE_RUN_CC = $(CC) -c $(CFLAGS) $(INCPATH) -o $obj $src');
    }
    if ( !Project("TMAKE_RUN_CC_IMP") ) {
	Project('TMAKE_RUN_CC_IMP = $(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<');
    }
    if ( !Project("TMAKE_RUN_CXX") ) {
	Project('TMAKE_RUN_CXX = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $obj $src');
    }
    if ( !Project("TMAKE_RUN_CXX_IMP") ) {
	Project('TMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<');
    }

    Project('TMAKE_FILETAGS = HEADERS SOURCES TARGET DESTDIR $$FILETAGS');
    Project('DESTDIR = ./') unless Project('DESTDIR');

    if ( Config("embedded") && Project("PRECOMPH") ) {
        Project('SOURCES += allmoc.cpp');
        $project{'HEADERS_ORIG'} = Project('HEADERS');
        $project{'HEADERS'} = "";
    }

    StdInit();

    if ( Config("qt") ) {
	if ( Project("TARGET") eq "qt"
	  || Project("TARGET") eq "qte" ) {
	    $project{"PNG_OBJECTS"} = &Objects($project{"PNG_SOURCES"});
	    $project{"ZLIB_OBJECTS"} = &Objects($project{"ZLIB_SOURCES"});
	    $project{"FT_OBJECTS"} = &Objects($project{"FT_SOURCES"});
	}
    }

    my ($module,$label);
    for $module ( split / /, $project{"MODULES"} ) {
	for $label ( "HEADERS", "OBJECTS", "OBJMOC", "SOURCES", "SRCMOC" ) {
	    my $o = "";
	    my $p = $project{$label};
	    while ( $p =~ s/\s*\b($module\/\S*)// ) {
		$o .= " " if $o;
		$o .= "$1";
	    }
	    $p =~ s/^ *//;
	    $p =~ s/ *$//;
	    $project{$label} = $p;
	    $project{"${label}_${module}"} = $o;
	}
    }

    $project{"DESTDIR"} = FixPath($project{"DESTDIR"});
    $project{"VERSION"} || ($project{"VERSION"} = "1.0.0");
    ($project{"VER_MAJ"},
     $project{"VER_MIN"},
     $project{"VER_PATCH"}) = $project{"VERSION"} =~ /(\d+)\.(\d+)(?:\.(\d+))?/;
    $project{"VER_PATCH"} = 0 if !$project{"VER_PATCH"};
    if ( Config("dll") ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_SHOBJ)' );
	Project('TMAKE_CFLAGS *= $(SYSCONF_CFLAGS_SHOBJ)' );
	Project('TMAKE_LFLAGS *= $(SYSCONF_LFLAGS_SHOBJ)');
    }
    if ( Config("yacc") ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_YACC)' );
	Project('TMAKE_LIBS *= $(SYSCONF_LIBS_YACC)' );
    }
    Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS)' );
    Project('TMAKE_CFLAGS *= $(SYSCONF_CFLAGS)' );
    Project('TMAKE_LFLAGS *= $(SYSCONF_LFLAGS)' );
    if ( Project('TEMPLATE') eq "lib" || Project('TEMPLATE') eq "qt.t" ) {
	Project('TMAKE_CXXFLAGS *= $(SYSCONF_CXXFLAGS_LIB)' );
	Project('TMAKE_CFLAGS *= $(SYSCONF_CFLAGS_LIB)' );
    } else {
	Project('TMAKE_LIBS *= $(SYSCONF_LIBS)' );
    }
#$}

####### Compiler, tools and options

CXX	=	$(SYSCONF_CXX) $(QT_CXX_MT)
CXXFLAGS=	#$ Expand("TMAKE_CXXFLAGS"); ExpandGlue("DEFINES","-D"," -D",""); (Project("TARGET") eq "qt" || Project("TARGET") eq "qte") && ($text = $text . ' $(QT_CXXFLAGS_OPT)');
CC	=	$(SYSCONF_CC) $(QT_C_MT)
CFLAGS	=	#$ Expand("TMAKE_CFLAGS"); ExpandGlue("DEFINES","-D"," -D",""); (Project("TARGET") eq "qt" || Project("TARGET") eq "qte") && ($text = $text . ' $(QT_CFLAGS_OPT)');
INCPATH =	#$ ExpandGlue("INCPATH","-I"," -I","");
LFLAGS	=	#$ Expand("TMAKE_LFLAGS"); $text .= ' $(QT_LFLAGS_MT)'
LIBS	=	$(SUBLIBS) #${
    Expand("TMAKE_LIBS");
    if (Project("TARGET") eq "qt" || Project("TARGET") eq "qte") {
	$text .= ' $(SYSCONF_LIBS_QTLIB)';
    } else {
	$text .= ' $(SYSCONF_LIBS_QTAPP)';
    }
#$}
MOC	=	$(SYSCONF_MOC)
UIC	=	$(SYSCONF_UIC)

####### Target

DESTDIR = #$ Expand("DESTDIR");
VER_MAJ = #$ Expand("VER_MAJ");
VER_MIN = #$ Expand("VER_MIN");
VER_PATCH = #$ Expand("VER_PATCH");
TARGET	= #$ Expand("TARGET"); $text .= '$(QT_THREAD_SUFFIX)' if Project("TARGET") eq "qt" || Project("TARGET") eq "qte";
TARGET1 = lib$(TARGET).so.$(VER_MAJ)

####### Files

HEADERS =	#$ ExpandList("HEADERS");
SOURCES =	#$ ExpandList("SOURCES");
OBJECTS =	#$ ExpandList("OBJECTS"); (Project("TARGET") eq "qt" || Project("TARGET") eq "qte") && ($text = $text . ' $(QT_MODULE_OBJ)');
INTERFACES =    #$ ExpandList("INTERFACES");
UICDECLS =      #$ ExpandList("UICDECLS");
UICIMPLS =      #$ ExpandList("UICIMPLS");
SRCMOC	=	#$ ExpandList("SRCMOC");
OBJMOC	=	#$ ExpandList("OBJMOC");
#$ if (Project("TARGET") ne "qt" && Project("TARGET") ne "qte") { DisableOutput(); }
PNG_OBJECTS  = #$ ExpandList("PNG_OBJECTS");
ZLIB_OBJECTS = #$ ExpandList("ZLIB_OBJECTS");
FT_OBJECTS = #$ ExpandList("FT_OBJECTS");
#$ if (Project("TARGET") ne "qt" && Project("TARGET") ne "qte") { EnableOutput(); }

#${
    my $module;
    $t = "";
    for $module ( split / /, $project{"MODULES"} ) {
	$t .= "\nOBJECTS_$module = ";
	$text = "";
	ExpandList("OBJECTS_$module");
	$t .= $text;
	if ( Project("OBJMOC_$module") ) {
	    $text = "";
	    ExpandList("OBJMOC_$module");
	    $t .= " \\\n\t\t$text";
	}
    }
    $text = "$t\n";
#$}

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cxx.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cc.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.C.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.c.o:
	#$ Expand("TMAKE_RUN_CC_IMP");

####### Build rules

#${
	if ( Project("SUBLIBS") ) {
	    $text = "SUBLIBS=";
	    for $m ( split / /, Project("SUBLIBS") ) {
		$text .= "tmp/lib$m.a ";
	    }
	    $text .= "\n";
	}
#$}

#${
	if ( Project('TEMPLATE') eq "lib" || Project('TEMPLATE') eq "qt.t" ) {
	    $targ='$(SYSCONF_LINK_TARGET)';
	} else {
	    if ( Config('dll') ) {
		if ($is_unix) {
		    $targ = '$(TARGET)' . ".so";
		} else {
		    $targ = '$(TARGET)' . ".dll";
		    $targ = "np" . $targ if Config('np');
		}
	    } else {
		$targ = '$(TARGET)';
	    }
	}
	$targ = '$(DESTDIR)'.$targ;
	$text .= 'all: ';
       	ExpandGlue("ALL_DEPS",""," "," ");
	$text .= $targ . "\n\n";
	$text .= $targ . ': $(UICDECLS) $(OBJECTS) $(OBJMOC) $(SUBLIBS)';
	Expand("TARGETDEPS");
	$text .= "\n\t";
	if ( Project('TEMPLATE') eq "lib" || Project('TEMPLATE') eq "qt.t" ) {
	    $text .= '$(SYSCONF_LINK_LIB)';
	} else {
	    $text .= '$(SYSCONF_LINK) $(LFLAGS) ';
	    $text .= '-o '.$targ.' $(OBJECTS) $(OBJMOC) $(LIBS)';
	}
#$}

moc: $(SRCMOC)

#$ TmakeSelf();

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS)
	#$ ExpandGlue("TMAKE_CLEAN","-rm -f "," ","");
	-rm -f *~ core
	#$ ExpandGlue("CLEAN_FILES","-rm -f "," ","");
	-rm -f allmoc.cpp

####### Extension Modules

listpromodules:
	@echo #$ ExpandList("MODULES_BASE"); ExpandList("MODULES_PRO");

listallmodules:
	@echo #$ ExpandList("MODULES");

listaddonpromodules:
	@echo #$ ExpandList("MODULES_PRO");

listaddonentmodules:
	@echo #$ ExpandList("MODULES_PRO"); ExpandList("MODULES_ENT");


REQUIRES=#$ $text .= Project("REQUIRES");

####### Sub-libraries

#${
	if ( Project("SUBLIBS") ) {
	    for $m ( split / /, Project("SUBLIBS") ) {
		$text .= "tmp/lib$m.a:\n\t";
		if ( $m eq "freetype" ) {
		    $text .= '$(MAKE) -C 3rdparty/freetype2 \
                            CONFIG_MK=config$(DASHCROSS).mk OBJ_DIR=../../tmp \
                            ../../tmp/libfreetype.a'."\n";
		} else {
		    if ( Config('embedded') ) {
		      if ( $m eq "z" ) {
			$text .= '$(MAKE) -C 3rdparty/zlib -f Makefile$(DASHCROSS); \
			cp 3rdparty/zlib/libz.a tmp'."\n";
		      }
		      if ( $m eq "png" ) {
			$text .= '$(MAKE) -C 3rdparty/libpng \
			    -f scripts/makefile.linux$(DASHCROSS); \
			    cp 3rdparty/libpng/libpng.a tmp'."\n";
		      }
		      if ( $m eq "mng" ) {
			$text .= '$(MAKE) -C 3rdparty/libmng \
			    -f makefiles/makefile.linux$(DASHCROSS); \
			    cp 3rdparty/libmng/libmng.a tmp'."\n";
		      }
		      if ( $m eq "jpeg" ) {
			$text .= '$(MAKE) -C 3rdparty/jpeglib -f makefile.unix$(DASHCROSS); \
			    cp 3rdparty/jpeglib/libjpeg.a tmp'."\n";
		      }
		    } else {
			    $text .= $project{"MAKELIB$m"}."\n";
		    }
		}

	    }
	}
#$}

###### Combined headers

#${
	if ( Config("embedded") && Project("PRECOMPH") ) {
	    $t = "allmoc.cpp: ".Project("PRECOMPH")." ".$original_HEADERS;
	    ExpandList("HEADERS_ORIG");
	    $t.= $text;
	    $t.= "\n\techo '#include \"".Project("PRECOMPH")."\"' >allmoc.cpp";
	    $t.= "\n\t\$(CXX) -E -DQT_MOC_CPP \$(CXXFLAGS) \$(INCPATH) >allmoc.h allmoc.cpp";
	    $t.= "\n\t\$(MOC) -o allmoc.cpp allmoc.h";
	    $t.= "\n\tperl -pi -e 's{\"allmoc.h\"}{\"".Project("PRECOMPH")."\"}' allmoc.cpp";
	    $t.= "\n\trm allmoc.h";
	    $t.= "\n";
	    $text = $t;
	}
#$}


####### Compile

#$ BuildObj(Project("OBJECTS"),Project("SOURCES"));
#$ BuildUicSrc(Project("INTERFACES"));
#$ BuildObj(Project("UICOBJECTS"),Project("UICIMPLS"));
#$ BuildMocObj(Project("OBJMOC"),Project("SRCMOC"));
#$ BuildMocSrc(Project("HEADERS"));
#$ BuildMocSrc(Project("SOURCES"));
#$ BuildMocSrc(Project("UICDECLS"));
#$ if (Project("TARGET") ne "qt" && Project("TARGET") ne "qte") { DisableOutput(); }
#$ Project("PNG_SOURCES") && BuildObj(Project("PNG_OBJECTS"),Project("PNG_SOURCES"));
#$ Project("ZLIB_SOURCES") && BuildObj(Project("ZLIB_OBJECTS"),Project("ZLIB_SOURCES"));
#$ Project("FT_SOURCES") && BuildObj(Project("FT_OBJECTS"),Project("FT_SOURCES"));
#$ if (Project("TARGET") ne "qt" && Project("TARGET") ne "qte") { EnableOutput(); }

#${
    my $module;
    $t = "";
    for $module ( split / /, $project{"MODULES"} ) {
	$t .= "\n# Module $module...\n";
	$text = ""; BuildMocObj(Project("OBJMOC_$module"),Project("SRCMOC_$module"));
	$t .= $text;
	$text = ""; BuildMocSrc(Project("HEADERS_$module"));
	$t .= $text;
	$text = ""; BuildMocSrc(Project("SOURCES_$module"));
	$t .= $text;
	$text = ""; BuildObj(Project("OBJECTS_$module"),Project("SOURCES_$module"));
	$t .= $text;
	$text = $t;
    }
#$}
