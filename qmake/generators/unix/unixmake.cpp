/****************************************************************************
** $Id: unixmake.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
**
** Definition of ________ class.
**
** Created : 970521
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "unixmake.h"
#include "option.h"
#include <qregexp.h>
#include <qfile.h>
#include <qdict.h>
#include <qdir.h>
#include <time.h>


void
UnixMakefileGenerator::init()
{
    if(init_flag)
	return;
    init_flag = TRUE;

    if(!project->isEmpty("QMAKE_FAILED_REQUIREMENTS")) /* no point */
	return;

    QStringList &configs = project->variables()["CONFIG"];
    /* this should probably not be here, but I'm using it to wrap the .t files */
    if(project->first("TEMPLATE") == "app")
	project->variables()["QMAKE_APP_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "lib")
	project->variables()["QMAKE_LIB_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "subdirs") {
	MakefileGenerator::init();
	if(project->isEmpty("MAKEFILE"))
	    project->variables()["MAKEFILE"].append("Makefile");
	if(project->isEmpty("QMAKE"))
	    project->variables()["QMAKE"].append("qmake");
	if(project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].findIndex("qmake_all") == -1)
	    project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].append("qmake_all");
	return; /* subdirs is done */
    }

    if( project->isEmpty("QMAKE_EXTENSION_SHLIB") ) {
	QString os = project->variables()["QMAKESPEC"].first().section( '-', 0, 0 );
	if ( os == "cygwin" ) {
	    project->variables()["QMAKE_EXTENSION_SHLIB"].append( "dll" );
	} else {
	    project->variables()["QMAKE_EXTENSION_SHLIB"].append( "so" );
	}
    }
    if( project->isEmpty("QMAKE_COPY_FILE") )
	project->variables()["QMAKE_COPY_FILE"].append( "$(COPY) -p" );
    if( project->isEmpty("QMAKE_COPY_DIR") )
	project->variables()["QMAKE_COPY_DIR"].append( "$(COPY) -pR" );
    //If the TARGET looks like a path split it into DESTDIR and the resulting TARGET 
    if(!project->isEmpty("TARGET")) {
	QString targ = project->first("TARGET");
	int slsh = QMAX(targ.findRev('/'), targ.findRev(Option::dir_sep));
	if(slsh != -1) {
	    if(project->isEmpty("DESTDIR"))
		project->values("DESTDIR").append("");
	    else if(project->first("DESTDIR").right(1) != Option::dir_sep)
		project->variables()["DESTDIR"] = project->first("DESTDIR") + Option::dir_sep;
	    project->variables()["DESTDIR"] = project->first("DESTDIR") + targ.left(slsh+1);
	    project->variables()["TARGET"] = targ.mid(slsh+1);
	}
    }

    project->variables()["QMAKE_ORIG_TARGET"] = project->variables()["TARGET"];

    bool is_qt = (project->first("TARGET") == "qt" || project->first("TARGET") == "qte" ||
		  project->first("TARGET") == "qt-mt" || project->first("TARGET") == "qte-mt");
    bool extern_libs = !project->isEmpty("QMAKE_APP_FLAG") ||
		       (!project->isEmpty("QMAKE_LIB_FLAG") &&
			project->isActiveConfig("dll")) || is_qt;
    if(!project->isActiveConfig("global_init_link_order"))
	project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    if ( (!project->isEmpty("QMAKE_LIB_FLAG") && !project->isActiveConfig("staticlib") ) ||
	 (project->isActiveConfig("qt") &&  project->isActiveConfig( "plugin" ) )) {
	if(configs.findIndex("dll") == -1) configs.append("dll");
    } else if ( !project->isEmpty("QMAKE_APP_FLAG") || project->isActiveConfig("dll") ) {
	configs.remove("staticlib");
    }
    if ( project->isActiveConfig("warn_off") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_WARN_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_WARN_OFF"];
    } else if ( project->isActiveConfig("warn_on") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_WARN_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_WARN_ON"];
    }
    if ( project->isActiveConfig("debug") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_DEBUG"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_DEBUG"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_DEBUG"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RELEASE"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RELEASE"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_RELEASE"];
    }
    if(!project->isEmpty("QMAKE_INCREMENTAL"))
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_INCREMENTAL"];
    else if(!project->isEmpty("QMAKE_LFLAGS_PREBIND") &&
	    !project->variables()["QMAKE_LIB_FLAG"].isEmpty() &&
	    project->isActiveConfig("dll"))
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_PREBIND"];
    if(!project->isEmpty("QMAKE_INCDIR"))
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR"];
    if(!project->isEmpty("QMAKE_LIBDIR")) {
	if ( !project->isEmpty("QMAKE_RPATH") )
	    project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue("QMAKE_LIBDIR", " " + var("QMAKE_RPATH"), 
								  " " + var("QMAKE_RPATH"), "");
	project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue( "QMAKE_LIBDIR", "-L", " -L", "" );
    }
    if ( extern_libs && (project->isActiveConfig("qt") || project->isActiveConfig("opengl")) ) {
	if(configs.findIndex("x11lib") == -1)
	    configs.append("x11lib");
	if ( project->isActiveConfig("opengl") && configs.findIndex("x11inc") == -1 )
	    configs.append("x11inc");
    }
    if ( project->isActiveConfig("x11") ) {
	if(configs.findIndex("x11lib") == -1)
	    configs.append("x11lib");
	if(configs.findIndex("x11inc") == -1)
	    configs.append("x11inc");
    }
    if ( project->isActiveConfig("qt") ) {
	if ( project->isActiveConfig("accessibility" ) )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_ACCESSIBILITY_SUPPORT");
	if ( project->isActiveConfig("tablet") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_TABLET_SUPPORT");
	if(configs.findIndex("moc")) configs.append("moc");
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_QT"];
	if ( !project->isActiveConfig("debug") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_NO_DEBUG");
	if ( !is_qt ) {
	    if ( !project->isEmpty("QMAKE_LIBDIR_QT") ) {
		if ( !project->isEmpty("QMAKE_RPATH") )
		    project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue("QMAKE_LIBDIR_QT", " " + var("QMAKE_RPATH"), 
									  " " + var("QMAKE_RPATH"), "");
		project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue("QMAKE_LIBDIR_QT", "-L", " -L", "");
	    }
	    if (project->isActiveConfig("thread") && !project->isEmpty("QMAKE_LIBS_QT_THREAD"))
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_THREAD"];
	    else
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	}
    }
    if ( project->isActiveConfig("thread") ) {
	if(project->isActiveConfig("qt"))
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_THREAD_SUPPORT");
	if ( !project->isEmpty("QMAKE_CFLAGS_THREAD"))
	    project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_THREAD"];
	if( !project->isEmpty("QMAKE_CXXFLAGS_THREAD"))
	    project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_THREAD"];
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_THREAD"];
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_THREAD"];
	if(!project->isEmpty("QMAKE_LFLAGS_THREAD"))
	    project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_THREAD"];
    }
    if ( project->isActiveConfig("opengl") ) {
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_OPENGL"];
	if(!project->isEmpty("QMAKE_LIBDIR_OPENGL")) 
	    project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue("QMAKE_LIBDIR_OPENGL", "-L", " -L", "");
	if ( is_qt )
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL_QT"];
	else
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
    }
    if(project->isActiveConfig("global_init_link_order"))
	project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    if ( project->isActiveConfig("x11sm") )
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_X11SM"];
    if ( project->isActiveConfig("dylib") )
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_DYNLOAD"];
    if ( project->isActiveConfig("x11inc") )
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_X11"];
    if ( project->isActiveConfig("x11lib") ) {
	if(!project->isEmpty("QMAKE_LIBDIR_X11"))
	    project->variables()["QMAKE_LIBDIR_FLAGS"] += varGlue("QMAKE_LIBDIR_X11", "-L", " -L", "");
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_X11"];
    }
    if ( project->isActiveConfig("moc") )
	setMocAware(TRUE);
    if ( project->isEmpty("QMAKE_RUN_CC") )
	project->variables()["QMAKE_RUN_CC"].append("$(CC) -c $(CFLAGS) $(INCPATH) -o $obj $src");
    if ( project->isEmpty("QMAKE_RUN_CC_IMP") )
	project->variables()["QMAKE_RUN_CC_IMP"].append("$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<");
    if ( project->isEmpty("QMAKE_RUN_CXX") )
	project->variables()["QMAKE_RUN_CXX"].append("$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $obj $src");
    if ( project->isEmpty("QMAKE_RUN_CXX_IMP") )
	project->variables()["QMAKE_RUN_CXX_IMP"].append("$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<");
    project->variables()["QMAKE_FILETAGS"] += QStringList::split("HEADERS SOURCES TARGET DESTDIR", " ");
    if ( !project->isEmpty("PRECOMPH") ) {
	initOutPaths(); 	// Need to fix MOC_DIR since we do this before init()
	QString allmoc = fileFixify(project->first("MOC_DIR") + "/allmoc.cpp", QDir::currentDirPath(), Option::output_dir);
	project->variables()["SOURCES"].prepend(allmoc);
	project->variables()["HEADERS_ORIG"] = project->variables()["HEADERS"];
	project->variables()["HEADERS"].clear();
    }
    if( project->isActiveConfig("GNUmake") && !project->isEmpty("QMAKE_CFLAGS_DEPS"))
	include_deps = TRUE; //do not generate deps

    MakefileGenerator::init();
    if ( project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
	if(!project->isEmpty("QMAKE_APP_FLAG")) {
	    if(project->isEmpty("DESTDIR"))
		project->values("DESTDIR").append("");
	    project->variables()["DESTDIR"].first() += project->variables()["TARGET"].first() +
						       ".app/Contents/MacOS/";
	    project->variables()["QMAKE_PKGINFO"].append(project->first("DESTDIR") + "../PkgInfo");
	    project->variables()["ALL_DEPS"] += project->first("QMAKE_PKGINFO");

	    QString plist = specdir() + QDir::separator() + "Info.plist." +
			    project->first("TEMPLATE");
	    if(QFile::exists(Option::fixPathToLocalOS(plist))) {
		project->variables()["QMAKE_INFO_PLIST"].append(plist);
		project->variables()["QMAKE_INFO_PLIST_OUT"].append(project->first("DESTDIR") +
								    "../Info.plist");
		project->variables()["ALL_DEPS"] += project->first("QMAKE_INFO_PLIST_OUT");
		if(!project->isEmpty("RC_FILE"))
		    project->variables()["ALL_DEPS"] += project->first("DESTDIR") +
							"../Resources/application.icns";
	    }
	}
    }

    if(!project->isEmpty("QMAKE_INTERNAL_INCLUDED_FILES"))
	project->variables()["DISTFILES"] += project->variables()["QMAKE_INTERNAL_INCLUDED_FILES"];
    project->variables()["DISTFILES"] += Option::mkfile::project_files;

    init2();
    project->variables()["QMAKE_INTERNAL_PRL_LIBS"] << "QMAKE_LIBDIR_FLAGS" << "QMAKE_LIBS";
    if(!project->isEmpty("QMAKE_MAX_FILES_PER_AR")) {
	bool ok;
	int max_files = project->first("QMAKE_MAX_FILES_PER_AR").toInt(&ok);
	QStringList ar_sublibs, objs = project->variables()["OBJECTS"] + project->variables()["OBJMOC"];
	if(ok && max_files > 5 && max_files < (int)objs.count()) {
	    int obj_cnt = 0, lib_cnt = 0;
	    QString lib;
	    for(QStringList::Iterator objit = objs.begin(); objit != objs.end(); ++objit) {
		if((++obj_cnt) >= max_files) {
		    if(lib_cnt) {
			lib.sprintf("lib%s-tmp%d.a", project->first("QMAKE_ORIG_TARGET").latin1(), lib_cnt);
			ar_sublibs << lib;
			obj_cnt = 0;
		    }
		    lib_cnt++;
		}
	    }
	}
	if(!ar_sublibs.isEmpty()) {
	    project->variables()["QMAKE_AR_SUBLIBS"] = ar_sublibs;
	    project->variables()["QMAKE_INTERNAL_PRL_LIBS"] << "QMAKE_AR_SUBLIBS";
	}
    }
}

QStringList
UnixMakefileGenerator::uniqueSetLFlags(const QStringList &list1, QStringList &list2)
{
    QStringList ret;
    for(QStringList::ConstIterator it = list1.begin(); it != list1.end(); ++it) {
	bool unique = TRUE;
	if((*it).startsWith("-")) {
	    if((*it).startsWith("-l") || (*it).startsWith("-L")) {
		unique = list2.findIndex((*it)) == -1;
	    } else if(project->isActiveConfig("macx") && (*it).startsWith("-framework")) {
		int as_one = TRUE;
		QString framework_in;
		if((*it).length() > 11) {
		    framework_in = (*it).mid(11);
		} else {
		    if(it != list1.end()) {
			++it;
			as_one = FALSE;
			framework_in = (*it);
		    }
		}
		if(!framework_in.isEmpty()) {
		    for(QStringList::ConstIterator outit = list2.begin(); outit != list2.end(); ++outit) {
			if((*outit).startsWith("-framework")) {
			    QString framework_out;
			    if((*outit).length() > 11) {
				framework_out = (*outit).mid(11);
			    } else {
				if(it != list2.end()) {
				    ++outit;
				    framework_out = (*outit);
				}
			    }
			    if(framework_out == framework_in) {
				unique = FALSE;
				break;
			    }
			}
		    }
		    if(unique) {
			unique = FALSE; //because I'm about to just insert it myself
			if(as_one) {
			    ret.append("-framework " + framework_in);
			} else {
			    ret.append("-framework");
			    ret.append(framework_in);
			}
		    }
		}
	    } else {
		unique = (list2.findIndex((*it)) == -1);
	    }
	} else if(QFile::exists((*it))) {
	    unique = (list2.findIndex((*it)) == -1);
	} 
	if(unique)
	    ret.append((*it));
    }
    return ret;
}


void
UnixMakefileGenerator::processPrlVariable(const QString &var, const QStringList &l)
{
    if(var == "QMAKE_PRL_LIBS") 
	project->variables()["QMAKE_CURRENT_PRL_LIBS"] += uniqueSetLFlags(l, project->variables()["QMAKE_LIBS"]);
    else
	MakefileGenerator::processPrlVariable(var, l);
}

void
UnixMakefileGenerator::processPrlFiles()
{
    QDict<void> processed;
    QPtrList<MakefileDependDir> libdirs;
    libdirs.setAutoDelete(TRUE);
    const QString lflags[] = { "QMAKE_LIBDIR_FLAGS", "QMAKE_LIBS", QString::null };
    for(int i = 0; !lflags[i].isNull(); i++) {
	for(bool ret = FALSE; TRUE; ret = FALSE) {
	    QStringList l_out;
	    QStringList &l = project->variables()[lflags[i]];
	    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
		project->variables()["QMAKE_CURRENT_PRL_LIBS"].clear();
		QString opt = (*it).stripWhiteSpace();;
		if(opt.startsWith("-")) {
		    if(opt.startsWith("-L")) {
			QString r = opt.right(opt.length() - 2), l = r;
			fixEnvVariables(l);
			libdirs.append(new MakefileDependDir(r.replace("\"",""),
							     l.replace("\"","")));
		    } else if(opt.startsWith("-l") && !processed[opt]) {
			QString lib = opt.right(opt.length() - 2), prl;
			for(MakefileDependDir *mdd = libdirs.first(); mdd; mdd = libdirs.next() ) {
			    prl = mdd->local_dir + Option::dir_sep + "lib" + lib + Option::prl_ext;
			    if(processPrlFile(prl)) {
				if(prl.startsWith(mdd->local_dir))
				    prl.replace(0, mdd->local_dir.length(), mdd->real_dir);
				QRegExp reg("^.*lib(" + lib + "[^./=]*)\\..*$");
				if(reg.exactMatch(prl))
				    prl = "-l" + reg.cap(1);
				opt = prl;
				processed.insert(opt, (void*)1);
				ret = TRUE;
				break;
			    }
			}
		    } else if(project->isActiveConfig("macx") && opt.startsWith("-framework")) {
			if(opt.length() > 11) {
			    opt = opt.mid(11);
			} else {
			    ++it;
			    opt = (*it);
			}
			QString prl = "/System/Library/Frameworks/" + opt +
				      ".framework/" + opt + Option::prl_ext;
			if(processPrlFile(prl)) 
			    ret = TRUE;
			l_out.append("-framework");
		    }
		    if(!opt.isEmpty())
			l_out.append(opt);
		    l_out += uniqueSetLFlags(project->variables()["QMAKE_CURRENT_PRL_LIBS"], l_out);
		} else {
		    if(!processed[opt] && processPrlFile(opt)) {
			processed.insert(opt, (void*)1);
			ret = TRUE;
		    }
		    if(!opt.isEmpty())
			l_out.append(opt);
		    l_out += uniqueSetLFlags(project->variables()["QMAKE_CURRENT_PRL_LIBS"], l_out);
		}
	    }
	    if(ret && l != l_out)
		l = l_out;
	    else
		break;
	}
    }
}

QString
UnixMakefileGenerator::defaultInstall(const QString &t)
{
    if(t != "target" || project->first("TEMPLATE") == "subdirs")
	return QString();

    bool resource = FALSE;
    QStringList &uninst = project->variables()[t + ".uninstall"];
    QString ret, destdir=fileFixify(project->first("DESTDIR"));
    QString targetdir = Option::fixPathToTargetOS(project->first("target.path"), FALSE);
    if(!destdir.isEmpty() && destdir.right(1) != Option::dir_sep)
	destdir += Option::dir_sep;
    targetdir = "$(INSTALL_ROOT)" + Option::fixPathToTargetOS(targetdir, FALSE);
    if(targetdir.right(1) != Option::dir_sep)
	targetdir += Option::dir_sep;

    QStringList links;
    QString target="$(TARGET)";
    if(project->first("TEMPLATE") == "app") {
	target = "$(QMAKE_TARGET)";
	if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
	    destdir += "../../../";
	    target += ".app";
	    resource = TRUE;
	}
    } else if(project->first("TEMPLATE") == "lib") {
	if(project->isActiveConfig("create_prl") && !project->isEmpty("QMAKE_INTERNAL_PRL_FILE")) {
	    QString dst_prl = project->first("QMAKE_INTERNAL_PRL_FILE");
	    int slsh = dst_prl.findRev('/');
	    if(slsh != -1)
		dst_prl = dst_prl.right(dst_prl.length() - slsh - 1);
	    dst_prl = targetdir + dst_prl;
	    ret += "-$(COPY) " + project->first("QMAKE_INTERNAL_PRL_FILE") + " " + dst_prl;
	    if(!uninst.isEmpty())
		uninst.append("\n\t");
	    uninst.append("-$(DEL_FILE) \"" + dst_prl + "\"");
	}
	QString os = project->variables()["QMAKESPEC"].first().section( '-', 0, 0 );
	if ( os != "cygwin" ) {
	    if ( !project->isActiveConfig("staticlib") && !project->isActiveConfig("plugin") ) {
		if ( os == "hpux" ) {
		    links << "$(TARGET0)";
		} else {
		    links << "$(TARGET0)" << "$(TARGET1)" << "$(TARGET2)";
	        }
	    }
	}
    }
    QString src_targ = target;
    if(!destdir.isEmpty())
	src_targ = Option::fixPathToTargetOS(destdir + target, FALSE);
    QString dst_targ = fileFixify(targetdir + target);
    if(!ret.isEmpty())
	ret += "\n\t";
    ret += QString(resource ? "-$(COPY_DIR)" : "-$(COPY)") + " \"" +
	   src_targ + "\" \"" + dst_targ + "\"";
    if(!project->isEmpty("QMAKE_STRIP")) {
	ret += "\n\t-" + var("QMAKE_STRIP");
	if(resource)
	    ret = " \"" + dst_targ + "/Contents/MacOS/$(QMAKE_TARGET)";
	else
	    ret += " \"" + dst_targ + "\"";
    }
    if(!uninst.isEmpty())
	uninst.append("\n\t");
    if(resource)
	uninst.append("-$(DEL_FILE) -r \"" + dst_targ + "\"");
    else
	uninst.append("-$(DEL_FILE) \"" + dst_targ + "\"");
    if(!links.isEmpty()) {
	for(QStringList::Iterator it = links.begin(); it != links.end(); it++) {
	    if(Option::target_mode == Option::TARG_WIN_MODE ||
	       Option::target_mode == Option::TARG_MAC9_MODE) {
	    } else if(Option::target_mode == Option::TARG_UNIX_MODE ||
		      Option::target_mode == Option::TARG_MACX_MODE) {
		QString link = Option::fixPathToTargetOS(destdir + (*it), FALSE);
		int lslash = link.findRev(Option::dir_sep);
		if(lslash != -1)
		    link = link.right(link.length() - (lslash + 1));
		QString dst_link = fileFixify(targetdir + link);
		ret += "\n\t-$(SYMLINK) \"$(TARGET)\" \"" + dst_link + "\"";
		if(!uninst.isEmpty())
		    uninst.append("\n\t");
		uninst.append("-$(DEL_FILE) \"" + dst_link + "\"");
	    }
	}
    }
    return ret;
}


