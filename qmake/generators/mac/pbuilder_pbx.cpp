/****************************************************************************
** $Id: pbuilder_pbx.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
**
** Definition of ________ class.
**
** Created : 970521
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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

#include "pbuilder_pbx.h"
#include "option.h"
#include <qdir.h>
#include <qdict.h>
#include <qregexp.h>
#include <stdlib.h>
#include <time.h>
#ifdef Q_OS_UNIX
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

// Note: this is fairly hacky, but it does the job...


ProjectBuilderMakefileGenerator::ProjectBuilderMakefileGenerator(QMakeProject *p) : UnixMakefileGenerator(p)
{

}

bool
ProjectBuilderMakefileGenerator::writeMakefile(QTextStream &t)
{
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	/* for now just dump, I need to generated an empty xml or something.. */
	fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
		var("QMAKE_FAILED_REQUIREMENTS").latin1());
	return TRUE;
    }

    project->variables()["MAKEFILE"].clear();
    project->variables()["MAKEFILE"].append("Makefile");
    if(project->first("TEMPLATE") == "app" || project->first("TEMPLATE") == "lib") {
	return writeMakeParts(t);
    } else if(project->first("TEMPLATE") == "subdirs") { 
	writeSubdirs(t, FALSE);
	return TRUE;
    }
    return FALSE;
}

bool
ProjectBuilderMakefileGenerator::writeMakeParts(QTextStream &t)
{
    int i;
    QStringList tmp;
    bool did_preprocess = FALSE;

    //HEADER
    t << "// !$*UTF8*$!" << "\n"
      << "{" << "\n"
      << "\t" << "archiveVersion = 1;" << "\n"
      << "\t" << "classes = {" << "\n" << "\t" << "};" << "\n"
      << "\t" << "objectVersion = " << pbuilderVersion() << ";" << "\n"
      << "\t" << "objects = {" << endl;

    //MAKE QMAKE equivlant
    if(!project->isActiveConfig("no_autoqmake") && project->projectFile() != "(stdin)") {
	QString mkfile = pbx_dir + Option::dir_sep + "qt_makeqmake.mak";
	QFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    QTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "QMAKE    = "	<<
		(project->isEmpty("QMAKE_QMAKE") ? QString("$(QTDIR)/bin/qmake") :
		 var("QMAKE_QMAKE")) << endl;
	    writeMakeQmake(mkt);
	    mkf.close();
	}
	QString phase_key = keyFor("QMAKE_PBX_MAKEQMAKE_BUILDPHASE");
	mkfile = fileFixify(mkfile, QDir::currentDirPath());
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(phase_key);
	t << "\t\t" << phase_key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "generatedFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"Qt Qmake\";" << "\n"
	  << "\t\t\t" << "neededFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	  << "\t\t\t" << "shellScript = \"make -C " << QDir::currentDirPath() <<
	    " -f " << mkfile << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //DUMP SOURCES
    QMap<QString, QStringList> groups;
    QString srcs[] = { "SOURCES", "SRCMOC", "UICIMPLS", QString::null };
    for(i = 0; !srcs[i].isNull(); i++) {
	tmp = project->variables()[srcs[i]];
	QStringList &src_list = project->variables()["QMAKE_PBX_" + srcs[i]];
	for(QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
	    QString file = fileFixify((*it));
	    if(file.endsWith(Option::moc_ext)) 
		continue;
	    bool in_root = TRUE;
	    QString src_key = keyFor(file);
	    if(!project->isActiveConfig("flat")) {
		QString flat_file = fileFixify(file, QDir::currentDirPath(), Option::output_dir, TRUE);
		if(QDir::isRelativePath(flat_file) && flat_file.find(Option::dir_sep) != -1) {
		    QString last_grp("QMAKE_PBX_" + srcs[i] + "_HEIR_GROUP");
		    QStringList dirs = QStringList::split(Option::dir_sep, flat_file);
		    dirs.pop_back(); //remove the file portion as it will be added via src_key
		    for(QStringList::Iterator dir_it = dirs.begin(); dir_it != dirs.end(); ++dir_it) {
			QString new_grp(last_grp + Option::dir_sep + (*dir_it)),
			        new_grp_key(keyFor(new_grp)), last_grp_key(keyFor(last_grp));
			if(dir_it == dirs.begin()) {
			    if(!groups.contains(new_grp)) 
				project->variables()["QMAKE_PBX_" + srcs[i]].append(new_grp_key);
			} else {
			    groups[last_grp] += new_grp_key;
			}
			last_grp = new_grp;
		    }
		    groups[last_grp] += src_key;
		    in_root = FALSE;
		}
	    }
	    if(in_root)
		src_list.append(src_key);
	    //source reference
	    t << "\t\t" << src_key << " = {" << "\n"
	      << "\t\t\t" << "isa = PBXFileReference;" << "\n"
	      << "\t\t\t" << "path = \"" << file << "\";" << "\n"
	      << "\t\t\t" << "refType = " << reftypeForFile(file) << ";" << "\n"
	      << "\t\t" << "};" << "\n";
	    //build reference
	    QString obj_key = file + ".o";
	    obj_key = keyFor(obj_key);
	    t << "\t\t" << obj_key << " = {" << "\n"
	      << "\t\t\t" << "fileRef = " << src_key << ";" << "\n"
	      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
	      << "\t\t\t" << "settings = {" << "\n"
	      << "\t\t\t\t" << "ATTRIBUTES = (" << "\n"
	      << "\t\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "};" << "\n"
	      << "\t\t" << "};" << "\n";
	    project->variables()["QMAKE_PBX_OBJ"].append(obj_key);
	}
	if(!src_list.isEmpty()) {
	    QString grp;
	    if(srcs[i] == "SOURCES") {
		if(project->first("TEMPLATE") == "app" && !project->isEmpty("RC_FILE")) { //Icon
		    QString icns_file = keyFor("ICNS_FILE");
		    src_list.append(icns_file);
		    t << "\t\t" << icns_file << " = {" << "\n"
		      << "\t\t\t" << "isa = PBXFileReference;" << "\n"
		      << "\t\t\t" << "path = \"" << project->first("RC_FILE") << "\";" << "\n"
		      << "\t\t\t" << "refType = " << reftypeForFile(project->first("RC_FILE")) << ";" << "\n"
		      << "\t\t" << "};" << "\n";
		    t << "\t\t" << keyFor("ICNS_FILE_REFERENCE") << " = {" << "\n"
		      << "\t\t\t" << "fileRef = " << icns_file << ";" << "\n"
		      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
		      << "\t\t\t" << "settings = {" << "\n"
		      << "\t\t\t" << "};" << "\n"
		      << "\t\t" << "};" << "\n";
		}
		grp = "Sources";
	    } else if(srcs[i] == "SRCMOC") {
		grp = "Mocables";
	    } else if(srcs[i] == "UICIMPLS") {
		grp = "UICables";
	    }
	    QString grp_key = keyFor(grp);
	    project->variables()["QMAKE_PBX_GROUPS"].append(grp_key);
	    t << "\t\t" << grp_key << " = {" << "\n"
	      << "\t\t\t" << "children = (" << "\n"
	      << varGlue("QMAKE_PBX_" + srcs[i], "\t\t\t\t", ",\n\t\t\t\t", "\n")
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "isa = PBXGroup;" << "\n"
	      << "\t\t\t" << "name = " << grp << ";" << "\n"
	      << "\t\t\t" << "refType = 4;" << "\n"
	      << "\t\t" << "};" << "\n";
	}
    }
    for(QMap<QString, QStringList>::Iterator grp_it = groups.begin(); 
	grp_it != groups.end(); ++grp_it) {
	t << "\t\t" << keyFor(grp_it.key()) << " = {" << "\n"
	  << "\t\t\t" << "isa = PBXGroup;" << "\n"
	  << "\t\t\t" << "children = (" << "\n"
	  << valGlue(grp_it.data(), "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "name = \"" << grp_it.key().section(Option::dir_sep, -1) << "\";" << "\n"
	  << "\t\t\t" << "refType = 4;" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //PREPROCESS BUILDPHASE (just a makefile)
    if(!project->isEmpty("UICIMPLS") || !project->isEmpty("SRCMOC") ||
	!project->isEmpty("YACCSOURCES") || !project->isEmpty("LEXSOURCES")) {
	QString mkfile = pbx_dir + Option::dir_sep + "qt_preprocess.mak";
	QFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    did_preprocess = TRUE;
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    QTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "MOC       = " << var("QMAKE_MOC") << endl;
	    mkt << "UIC       = " << var("QMAKE_UIC") << endl;
	    mkt << "LEX       = " << var("QMAKE_LEX") << endl;
	    mkt << "LEXFLAGS  = " << var("QMAKE_LEXFLAGS") << endl;
	    mkt << "YACC      = " << var("QMAKE_YACC") << endl;
	    mkt << "YACCFLAGS = " << var("QMAKE_YACCFLAGS") << endl;
	    mkt << "DEL_FILE  = " << var("QMAKE_DEL_FILE") << endl;
	    mkt << "MOVE      = " << var("QMAKE_MOVE") << endl << endl;
	    mkt << "FORMS = " << varList("UICIMPLS") << endl;
	    mkt << "MOCS = " << varList("SRCMOC") << endl;
	    mkt << "PARSERS =";
	    if(!project->isEmpty("YACCSOURCES")) {
		QStringList &yaccs = project->variables()["YACCSOURCES"];
		for(QStringList::Iterator yit = yaccs.begin(); yit != yaccs.end(); ++yit) {
		    QFileInfo fi((*yit));
		    mkt << " " << fi.dirPath() << Option::dir_sep << fi.baseName(TRUE)
			<< Option::yacc_mod << Option::cpp_ext.first();
		}
	    }
	    if(!project->isEmpty("LEXSOURCES")) {
		QStringList &lexs = project->variables()["LEXSOURCES"];
		for(QStringList::Iterator lit = lexs.begin(); lit != lexs.end(); ++lit) {
		    QFileInfo fi((*lit));
		    mkt << " " << fi.dirPath() << Option::dir_sep << fi.baseName(TRUE)
			<< Option::lex_mod << Option::cpp_ext.first();
		}
	    }
	    mkt << "\n";
	    mkt << "preprocess: $(FORMS) $(MOCS) $(PARSERS)" << endl;
	    mkt << "preprocess_clean: mocclean uiclean parser_clean" << endl << endl;
	    mkt << "mocclean:" << "\n";
	    if(!project->isEmpty("SRCMOC"))
		mkt << "\t-rm -f $(MOCS)" << "\n";
	    mkt << "uiclean:" << "\n";
	    if(!project->isEmpty("UICIMPLS"))
		mkt << "\t-rm -f $(FORMS)" << "\n";
	    mkt << "parser_clean:" << "\n";
	    if(!project->isEmpty("YACCSOURCES") || !project->isEmpty("LEXSOURCES"))
		mkt << "\t-rm -f $(PARSERS)" << "\n";
	    writeUicSrc(mkt, "FORMS");
	    writeMocSrc(mkt, "HEADERS");
	    writeMocSrc(mkt, "SOURCES");
	    writeMocSrc(mkt, "UICDECLS");
	    writeYaccSrc(mkt, "YACCSOURCES");
	    writeLexSrc(mkt, "LEXSOURCES");
	    mkf.close();
	}
	QString target_key = keyFor("QMAKE_PBX_PREPROCESS_TARGET");
	mkfile = fileFixify(mkfile, QDir::currentDirPath());
	t << "\t\t" << target_key << " = {" << "\n"
	  << "\t\t\t" << "buildArgumentsString = \"-f " << mkfile << "\";" << "\n"
	  << "\t\t\t" << "buildPhases = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "buildSettings = {" << "\n"
	  << "\t\t\t" << "};" << "\n"
	  << "\t\t\t" << "buildToolPath = \"/usr/bin/gnumake\";"<< "\n"
	  << "\t\t\t" << "buildWorkingDirectory = \"" << QDir::currentDirPath() << "\";" << "\n"
	  << "\t\t\t" << "dependencies = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXLegacyTarget;" << "\n"
	  << "\t\t\t" << "name = QtPreprocessors;" << "\n"
	  << "\t\t\t" << "productName = QtPreprocessors;" << "\n"
	  << "\t\t\t" << "settingsToExpand = 6;" << "\n"
	  << "\t\t\t" << "settingsToPassInEnvironment = 287;" << "\n"
	  << "\t\t\t" << "settingsToPassOnCommandLine = 280;" << "\n"
	  << "\t\t\t" << "shouldsUseHeadermap = 0;" << "\n"
	  << "\t\t" << "};" << "\n";

	QString target_depend_key = keyFor("QMAKE_PBX_PREPROCESS_TARGET_DEPEND");
	project->variables()["QMAKE_PBX_TARGETDEPENDS"].append(target_depend_key);
	t << "\t\t" << target_depend_key << " = {" << "\n"
	  << "\t\t\t" << "isa = PBXTargetDependency;" << "\n"
	  << "\t\t\t" << "target = " << target_key << ";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    //SOURCE BUILDPHASE
    if(!project->isEmpty("QMAKE_PBX_OBJ")) {
	QString grp = "Build Sources", key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << varGlue("QMAKE_PBX_OBJ", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXSourcesBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    if(!project->isActiveConfig("staticlib")) { //DUMP LIBRARIES
	QStringList &libdirs = project->variables()["QMAKE_PBX_LIBPATHS"];
	QString libs[] = { "QMAKE_LIBDIR_FLAGS", "QMAKE_LIBS", QString::null };
	for(i = 0; !libs[i].isNull(); i++) {
	    tmp = project->variables()[libs[i]];
	    for(QStringList::Iterator it = tmp.begin(); it != tmp.end();) {
		bool remove = FALSE;
		QString library, name, opt = (*it).stripWhiteSpace();
		if(opt.startsWith("-L")) {
		    QString r = opt.right(opt.length() - 2);
		    fixEnvVariables(r);
		    libdirs.append(r);
		} else if(opt.startsWith("-l")) {
		    name = opt.right(opt.length() - 2);
		    QString lib("lib" + name);
		    for(QStringList::Iterator lit = libdirs.begin(); lit != libdirs.end(); ++lit) {
			if(project->isActiveConfig("link_prl")) {
			    /* This isn't real nice, but it is real usefull. This looks in a prl
			       for what the library will ultimately be called so we can stick it
			       in the ProjectFile. If the prl format ever changes (not likely) then
			       this will not really work. However, more concerning is that it will
			       encode the version number in the Project file which might be a bad
			       things in days to come? --Sam
			    */
			    QString prl_file = (*lit) + Option::dir_sep + lib + Option::prl_ext;
			    if(QFile::exists(prl_file)) {
				QMakeProject proj;
				if(proj.read(prl_file, QDir::currentDirPath())) {
				    if(!proj.isEmpty("QMAKE_PRL_TARGET")) {
					library = (*lit) + Option::dir_sep + proj.first("QMAKE_PRL_TARGET");
					debug_msg(1, "pbuilder: Found library (%s) via PRL %s (%s)", 
						  opt.latin1(), prl_file.latin1(), library.latin1());
					remove = TRUE;
				    }
				}

			    }
			}
			if(!remove) {
			    QString extns[] = { ".dylib", ".so", ".a", QString::null };
			    for(int n = 0; !remove && !extns[n].isNull(); n++) {
				QString tmp =  (*lit) + Option::dir_sep + lib + extns[n];
				if(QFile::exists(tmp)) {
				    library = tmp;
				    debug_msg(1, "pbuilder: Found library (%s) via %s", 
					      opt.latin1(), library.latin1());
				    remove = TRUE;
				}
			    }
			}
		    }
		} else if(opt == "-framework") {
		    ++it;
		    if(it == tmp.end())
			break;
		    QStringList &fdirs = project->variables()["QMAKE_FRAMEWORKDIR"];
		    if(fdirs.isEmpty())
			fdirs.append("/System/Library/Frameworks/");
		    for(QStringList::Iterator fit = fdirs.begin(); fit != fdirs.end(); ++fit) {
			if(QFile::exists((*fit) + QDir::separator() + (*it) + ".framework")) {
			    --it;
			    it = tmp.remove(it);
			    remove = TRUE;
			    library = (*fit) + Option::dir_sep + (*it) + ".framework";
			    break;
			}
		    }
		} else if(opt.left(1) != "-") {
		    remove = TRUE;
		    library = opt;
		}
		if(!library.isEmpty()) {
		    if(name.isEmpty()) {
			int slsh = library.findRev(Option::dir_sep);
			if(slsh != -1)
			    name = library.right(library.length() - slsh - 1);
		    }
		    library = fileFixify(library);
		    QString key = keyFor(library);
		    bool is_frmwrk = (library.endsWith(".framework"));
		    t << "\t\t" << key << " = {" << "\n"
		      << "\t\t\t" << "isa = " << (is_frmwrk ? "PBXFrameworkReference" : "PBXFileReference") << ";" << "\n"
		      << "\t\t\t" << "name = \"" << name << "\";" << "\n"
		      << "\t\t\t" << "path = \"" << library << "\";" << "\n"
		      << "\t\t\t" << "refType = " << reftypeForFile(library) << ";" << "\n"
		      << "\t\t" << "};" << "\n";
		    project->variables()["QMAKE_PBX_LIBRARIES"].append(key);
		    QString obj_key = library + ".o";
		    obj_key = keyFor(obj_key);
		    t << "\t\t" << obj_key << " = {" << "\n"
		      << "\t\t\t" << "fileRef = " << key << ";" << "\n"
		      << "\t\t\t" << "isa = PBXBuildFile;" << "\n"
		      << "\t\t\t" << "settings = {" << "\n"
		      << "\t\t\t" << "};" << "\n"
		      << "\t\t" << "};" << "\n";
		    project->variables()["QMAKE_PBX_BUILD_LIBRARIES"].append(obj_key);
		}
		if(remove)
		    it = tmp.remove(it);
		else
		    ++it;
	    }
	    project->variables()[libs[i]] = tmp;
	}
    }
    //SUBLIBS BUILDPHASE (just another makefile)
    if(!project->isEmpty("SUBLIBS")) {
	QString mkfile = pbx_dir + Option::dir_sep + "qt_sublibs.mak";
	QFile mkf(mkfile);
	if(mkf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", mkfile.latin1());
	    QTextStream mkt(&mkf);
	    writeHeader(mkt);
	    mkt << "SUBLIBS= ";
	    tmp = project->variables()["SUBLIBS"];
	    QStringList::Iterator it;
	    for(it = tmp.begin(); it != tmp.end(); ++it)
		t << "tmp/lib" << (*it) << ".a ";
	    t << endl << endl;
	    mkt << "sublibs: $(SUBLIBS)" << endl << endl;
	    tmp = project->variables()["SUBLIBS"];
	    for(it = tmp.begin(); it != tmp.end(); ++it)
		t << "tmp/lib" << (*it) << ".a" << ":\n\t"
		  << var(QString("MAKELIB") + (*it)) << endl << endl;
	    mkf.close();
	}
	QString phase_key = keyFor("QMAKE_PBX_SUBLIBS_BUILDPHASE");
	mkfile = fileFixify(mkfile, QDir::currentDirPath());
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(phase_key);
	t << "\t\t" << phase_key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "generatedFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"Qt Sublibs\";" << "\n"
	  << "\t\t\t" << "neededFileNames = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	  << "\t\t\t" << "shellScript = \"make -C " << QDir::currentDirPath() <<
	    " -f " << mkfile << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    //LIBRARY BUILDPHASE
    if(!project->isEmpty("QMAKE_PBX_LIBRARIES")) {
	tmp = project->variables()["QMAKE_PBX_LIBRARIES"];
	if(!tmp.isEmpty()) {
	    QString grp("External Frameworks and Libraries"), key = keyFor(grp);
	    project->variables()["QMAKE_PBX_GROUPS"].append(key);
	    t << "\t\t" << key << " = {" << "\n"
	      << "\t\t\t" << "children = (" << "\n"
	      << varGlue("QMAKE_PBX_LIBRARIES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "isa = PBXGroup;" << "\n"
	      << "\t\t\t" << "name = \"" << grp << "\"" << ";" << "\n"
	      << "\t\t\t" << "path = \"\";" << "\n"
	      << "\t\t\t" << "refType = 4;" << "\n"
	      << "\t\t" << "};" << "\n";
	}
    }
    {
	QString grp("Frameworks & Libraries"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << varGlue("QMAKE_PBX_BUILD_LIBRARIES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXFrameworksBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console") &&
       project->first("TEMPLATE") == "app") { //BUNDLE RESOURCES
	QString grp("Bundle Resources"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_BUILDPHASES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildActionMask = 2147483647;" << "\n"
	  << "\t\t\t" << "files = (" << "\n"
	  << (!project->isEmpty("RC_FILE") ? keyFor("ICNS_FILE_REFERENCE") : QString(""))
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXResourcesBuildPhase;" << "\n"
	  << "\t\t\t" << "name = \"" << grp << "\";" << "\n"
	  << "\t\t" << "};" << "\n";
    }

    //DUMP EVERYTHING THAT TIES THE ABOVE TOGETHER
    //PRODUCTS
    {
	QString grp("Products"), key = keyFor(grp);
	project->variables()["QMAKE_PBX_GROUPS"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "children = (" << "\n"
	  << "\t\t\t\t" << keyFor("QMAKE_PBX_REFERENCE") << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "isa = PBXGroup;" << "\n"
	  << "\t\t\t" << "name = Products;" << "\n"
	  << "\t\t\t" << "refType = 4;" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    { //INSTALL BUILDPHASE (sh script)
	QString targ = project->first("TARGET");
	if(project->first("TEMPLATE") == "app" ||
	   (project->first("TEMPLATE") == "lib" && !project->isActiveConfig("staticlib") &&
	    project->isActiveConfig("frameworklib")))
	    targ = project->first("QMAKE_ORIG_TARGET");
	int slsh = targ.findRev(Option::dir_sep);
	if(slsh != -1)
	    targ = targ.right(targ.length() - slsh - 1);
	fixEnvVariables(targ);
	QStringList links;
	if(project->first("TEMPLATE") == "app") {
	    if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console"))
		targ += ".app";
	} else if(!project->isActiveConfig("staticlib") &&
	   !project->isActiveConfig("frameworklib")) {
	    QString li[] = { "TARGET_", "TARGET_x", "TARGET_x.y", QString::null };
	    for(int n = 0; !li[n].isNull(); n++) {
		QString t = project->first(li[n]);
		slsh = t.findRev(Option::dir_sep);
		if(slsh != -1)
		    t = t.right(t.length() - slsh);
		fixEnvVariables(t);
		links << t;
	    }
	}
	QString script = pbx_dir + Option::dir_sep + "qt_install.sh";
	QFile shf(script);
	if(shf.open(IO_WriteOnly | IO_Translate)) {
	    debug_msg(1, "pbuilder: Creating file: %s", script.latin1());
	    QString targ = project->first("QMAKE_ORIG_TARGET"), cpflags;
	    if(project->first("TEMPLATE") == "app") {
		targ = project->first("TARGET");
		if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
		    targ += ".app";
		    cpflags += "-r ";
		}
	    } else if(!project->isActiveConfig("frameworklib")) {
		if(project->isActiveConfig("staticlib"))
		    targ = project->first("TARGET");
		else
		    targ = project->first("TARGET_");
		int slsh = targ.findRev(Option::dir_sep);
		if(slsh != -1)
		    targ = targ.right(targ.length() - slsh - 1);
	    }
	    QTextStream sht(&shf);
	    QString dstdir = project->first("DESTDIR");
	    fixEnvVariables(dstdir);

	    sht << "#!/bin/sh" << endl;
	    //copy the actual target
	    sht << "OUT_TARG=\"" << targ << "\"\n" 
		<< "[ -z \"$BUILD_ROOT\" ] || OUT_TARG=\"${BUILD_ROOT}/${OUT_TARG}\"" << endl;
	    sht << "[ \"$OUT_TARG\" = \"" 
		<< (dstdir.isEmpty() ? QDir::currentDirPath() + QDir::separator(): dstdir) << targ << "\" ] || " 
		<< "[ \"$OUT_TARG\" = \"" << targ << "\" ] || " 
		<< "cp -r \"$OUT_TARG\" " << "\"" << dstdir << targ << "\"" << endl;
	    //rename as a framework
	    if(project->first("TEMPLATE") == "lib" && project->isActiveConfig("frameworklib"))
		sht << "ln -sf \"" << targ <<  "\" " << "\"" << dstdir << targ << "\"" << endl;
	    //create all the version symlinks (just to be like unixmake)
	    for(QStringList::Iterator it = links.begin(); it != links.end(); ++it) {
		if(targ != (*it)) 
		    sht << "ln -sf \"" << targ <<  "\" " << "\"" << dstdir << (*it) << "\"" << endl;
	    }
	    shf.close();
#ifdef Q_OS_UNIX
	    chmod(script.latin1(), S_IRWXU | S_IRWXG);
#endif
	    QString phase_key = keyFor("QMAKE_PBX_INSTALL_BUILDPHASE");
	    script = fileFixify(script, QDir::currentDirPath());
	    project->variables()["QMAKE_PBX_BUILDPHASES"].append(phase_key);
	    t << "\t\t" << phase_key << " = {" << "\n"
	      << "\t\t\t" << "buildActionMask = 8;" << "\n" //only on install!
	      << "\t\t\t" << "files = (" << "\n"
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "generatedFileNames = (" << "\n"
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "isa = PBXShellScriptBuildPhase;" << "\n"
	      << "\t\t\t" << "name = \"Qt Install\";" << "\n"
	      << "\t\t\t" << "neededFileNames = (" << "\n"
	      << "\t\t\t" << ");" << "\n"
	      << "\t\t\t" << "shellPath = /bin/sh;" << "\n"
	      << "\t\t\t" << "shellScript = \"" << script << "\";" << "\n"
	      << "\t\t" << "};" << "\n";
	}
    }
    //ROOT_GROUP
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT_GROUP") << " = {" << "\n"
      << "\t\t\t" << "children = (" << "\n"
      << varGlue("QMAKE_PBX_GROUPS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXGroup;" << "\n"
      << "\t\t\t" << "name = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n"
      << "\t\t\t" << "path = \"\";" << "\n"
      << "\t\t\t" << "refType = 4;" << "\n"
      << "\t\t" << "};" << "\n";
    //REFERENCE
    t << "\t\t" << keyFor("QMAKE_PBX_REFERENCE") << " = {" << "\n";
    if(project->first("TEMPLATE") == "app") {
	QString targ = project->first("QMAKE_ORIG_TARGET");
	if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
	    targ += ".app";
	    t << "\t\t\t" << "isa = PBXApplicationReference;" << "\n";
	} else {
	    t << "\t\t\t" << "isa = PBXExecutableFileReference;" << "\n";
	}
	QString app = (!project->isEmpty("DESTDIR") ? project->first("DESTDIR") + project->first("QMAKE_ORIG_TARGET") : 
		       QDir::currentDirPath()) + Option::dir_sep + targ;
	t << "\t\t\t" << "name = " <<  targ << ";" << "\n"
	  << "\t\t\t" << "path = \"" << targ << "\";" << "\n"
	  << "\t\t\t" << "refType = " << reftypeForFile(app) << ";" << "\n";
    } else {
	QString lib = project->first("QMAKE_ORIG_TARGET");
	if(project->isActiveConfig("staticlib")) {
	    lib = project->first("TARGET");
	} else if(!project->isActiveConfig("frameworklib")) {
	    if(project->isActiveConfig("plugin"))
		lib = project->first("TARGET");
	    else
		lib = project->first("TARGET_");
	}
	int slsh = lib.findRev(Option::dir_sep);
	if(slsh != -1)
	    lib = lib.right(lib.length() - slsh - 1);
	t << "\t\t\t" << "isa = PBXLibraryReference;" << "\n"
	  << "\t\t\t" << "path = " << lib << ";\n"
	  << "\t\t\t" << "refType = " << reftypeForFile(lib) << ";" << "\n";
    }
    t << "\t\t" << "};" << "\n";
    //TARGET
    t << "\t\t" << keyFor("QMAKE_PBX_TARGET") << " = {" << "\n"
      << "\t\t\t" << "buildPhases = (" << "\n"
      << varGlue("QMAKE_PBX_BUILDPHASES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "buildSettings = {" << "\n"
      << "\t\t\t\t" << "FRAMEWORK_SEARCH_PATHS = \"\";" << "\n"
      << "\t\t\t\t" << "HEADER_SEARCH_PATHS = \"" << fixEnvsList("INCLUDEPATH") << " " << fixEnvs(specdir()) << "\";" << "\n"
      << "\t\t\t\t" << "LIBRARY_SEARCH_PATHS = \"" << var("QMAKE_PBX_LIBPATHS") << "\";" << "\n"
      << "\t\t\t\t" << "OPTIMIZATION_CFLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "OTHER_CFLAGS = \"" <<
	fixEnvsList("QMAKE_CFLAGS") << varGlue("PRL_EXPORT_DEFINES"," -D"," -D","") <<
	varGlue("DEFINES"," -D"," -D","") << "\";" << "\n"
      << "\t\t\t\t" << "LEXFLAGS = \"" << var("QMAKE_LEXFLAGS") << "\";" << "\n"
      << "\t\t\t\t" << "YACCFLAGS = \"" << var("QMAKE_YACCFLAGS") << "\";" << "\n"
      << "\t\t\t\t" << "OTHER_CPLUSPLUSFLAGS = \"" <<
	fixEnvsList("QMAKE_CXXFLAGS") << varGlue("PRL_EXPORT_DEFINES"," -D"," -D","") <<
	varGlue("DEFINES"," -D"," -D","") << "\";" << "\n"
      << "\t\t\t\t" << "OTHER_REZFLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "SECTORDER_FLAGS = \"\";" << "\n"
      << "\t\t\t\t" << "WARNING_CFLAGS = \"\";" << "\n";
#if 1
    t << "\t\t\t\t" << "BUILD_ROOT = \"" << QDir::currentDirPath() << "\";" << "\n";
#endif
    if(!project->isActiveConfig("staticlib")) 
	t << "\t\t\t\t" << "OTHER_LDFLAGS = \"" << fixEnvsList("SUBLIBS") << " " <<
	    fixEnvsList("QMAKE_LFLAGS") << " " << fixEnvsList("QMAKE_LIBDIR_FLAGS") <<
	    " " << fixEnvsList("QMAKE_LIBS") << "\";" << "\n";
    if(!project->isEmpty("DESTDIR"))
	t << "\t\t\t\t" << "INSTALL_PATH = \"" << project->first("DESTDIR") << "\";" << "\n";
    if(!project->isEmpty("VERSION") && project->first("VERSION") != "0.0.0")
	t << "\t\t\t\t" << "DYLIB_CURRENT_VERSION = \"" << project->first("VERSION") << "\";" << "\n";
    if(!project->isEmpty("OBJECTS_DIR"))
	t << "\t\t\t\t" << "OBJECT_FILE_DIR = \"" << project->first("OBJECTS_DIR") << "\";" << "\n";
    if(project->first("TEMPLATE") == "app") {
	if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console"))
	    t << "\t\t\t\t" << "WRAPPER_EXTENSION = app;" << "\n";
	t << "\t\t\t\t" << "PRODUCT_NAME = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n";
    } else {
	QString lib = project->first("QMAKE_ORIG_TARGET");
	if(!project->isActiveConfig("plugin") && project->isActiveConfig("staticlib")) {
	    t << "\t\t\t\t" << "LIBRARY_STYLE = STATIC;" << "\n";
	    lib = project->first("TARGET");
	} else {
	    t << "\t\t\t\t" << "LIBRARY_STYLE = DYNAMIC;" << "\n";
	    if(!project->isActiveConfig("frameworklib")) {
		if(project->isActiveConfig("plugin"))
		    lib = project->first("TARGET");
		else
		    lib = project->first("TARGET_");
	    }
	}
	int slsh = lib.findRev(Option::dir_sep);
	if(slsh != -1)
	    lib = lib.right(lib.length() - slsh - 1);
	t << "\t\t\t\t" << "PRODUCT_NAME = " << lib << ";" << "\n";
    }
    tmp = project->variables()["QMAKE_PBX_VARS"];
    for(QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it)
	t << "\t\t\t\t" << (*it) << " = \"" << getenv((*it)) << "\";" << "\n";
    t << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "conditionalBuildSettings = {" << "\n"
      << "\t\t\t" << "};" << "\n"
      << "\t\t\t" << "dependencies = (" << "\n"
      << varGlue("QMAKE_PBX_TARGETDEPENDS", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "productReference = " << keyFor("QMAKE_PBX_REFERENCE") << ";" << "\n"
      << "\t\t\t" << "shouldUseHeadermap = 1;" << "\n";
    if(project->first("TEMPLATE") == "app") {
	if(project->isActiveConfig("resource_fork") && !project->isActiveConfig("console")) {
	    t << "\t\t\t" << "isa = PBXApplicationTarget;" << "\n"
	      << "\t\t\t" << "productSettingsXML = " << "\"" << "<?xml version=" 
	      << "\\\"1.0\\\" encoding=" << "\\\"UTF-8\\\"" << "?>" << "\n"
	      << "\t\t\t\t" << "<!DOCTYPE plist SYSTEM \\\"file://localhost/System/" 
	      << "Library/DTDs/PropertyList.dtd\\\">" << "\n"
	      << "\t\t\t\t" << "<plist version=\\\"0.9\\\">" << "\n"
	      << "\t\t\t\t" << "<dict>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleDevelopmentRegion</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>English</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleExecutable</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>" << project->first("QMAKE_ORIG_TARGET") << "</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleIconFile</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>" << var("RC_FILE").section(Option::dir_sep, -1) << "</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleInfoDictionaryVersion</key>"  << "\n"
	      << "\t\t\t\t\t" << "<string>6.0</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundlePackageType</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>APPL</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleSignature</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>????</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CFBundleVersion</key>" << "\n"
	      << "\t\t\t\t\t" << "<string>0.1</string>" << "\n"
	      << "\t\t\t\t\t" << "<key>CSResourcesFileMapped</key>" << "\n"
	      << "\t\t\t\t\t" << "<true/>" << "\n"
	      << "\t\t\t\t" << "</dict>" << "\n"
	      << "\t\t\t\t" << "</plist>" << "\";" << "\n";
	} else {
	    t << "\t\t\t" << "isa = PBXToolTarget;" << "\n";
	}
	t << "\t\t\t" << "name = \"" << project->first("QMAKE_ORIG_TARGET") << "\";" << "\n"
	  << "\t\t\t" << "productName = " << project->first("QMAKE_ORIG_TARGET") << ";" << "\n";
    } else {
	QString lib = project->first("QMAKE_ORIG_TARGET");
	if(!project->isActiveConfig("frameworklib"))
	   lib.prepend("lib");
	t << "\t\t\t" << "isa = PBXLibraryTarget;" << "\n"
	  << "\t\t\t" << "name = \"" << lib << "\";" << "\n"
	  << "\t\t\t" << "productName = " << lib << ";" << "\n";
    }
    if(!project->isEmpty("DESTDIR"))
	t << "\t\t\t" << "productInstallPath = \"" << project->first("DESTDIR") << "\";" << "\n";
    t << "\t\t" << "};" << "\n";
    //DEBUG/RELEASE
    for(i = 0; i < 2; i++) {
	bool as_release = !i;
	if(project->isActiveConfig("debug")) 
	    as_release = i;
	QString key = "QMAKE_PBX_" + QString(as_release ? "RELEASE" : "DEBUG");
	key = keyFor(key);
	project->variables()["QMAKE_PBX_BUILDSTYLES"].append(key);
	t << "\t\t" << key << " = {" << "\n"
	  << "\t\t\t" << "buildRules = (" << "\n"
	  << "\t\t\t" << ");" << "\n"
	  << "\t\t\t" << "buildSettings = {" << "\n"
	  << "\t\t\t\t" << "COPY_PHASE_STRIP = " << (as_release ? "YES" : "NO") << ";" << "\n";
	if(as_release) 
	    t << "\t\t\t\t" << "DEBUGGING_SYMBOLS = NO;" << "\n";
	t << "\t\t\t" << "};" << "\n"
	  << "\t\t\t" << "isa = PBXBuildStyle;" << "\n"
	  << "\t\t\t" << "name = " << (as_release ? "Deployment" : "Development") << ";" << "\n"
	  << "\t\t" << "};" << "\n";
    }
    //ROOT
    t << "\t\t" << keyFor("QMAKE_PBX_ROOT") << " = {" << "\n"
      << "\t\t\t" << "buildStyles = (" << "\n"
      << varGlue("QMAKE_PBX_BUILDSTYLES", "\t\t\t\t", ",\n\t\t\t\t", "\n")
      << "\t\t\t" << ");" << "\n"
      << "\t\t\t" << "isa = PBXProject;" << "\n"
      << "\t\t\t" << "mainGroup = " << keyFor("QMAKE_PBX_ROOT_GROUP") << ";" << "\n"
      << "\t\t\t" << "targets = (" << "\n"
      << "\t\t\t\t" << keyFor("QMAKE_PBX_TARGET") << "\n"
      << "\t\t\t" << ");" << "\n"
      << "\t\t" << "};" << "\n";

    //FOOTER
    t << "\t" << "};" << "\n"
      << "\t" << "rootObject = " << keyFor("QMAKE_PBX_ROOT") << ";" << "\n"
      << "}" << endl;

    QString mkwrap = fileFixify(pbx_dir + Option::dir_sep + ".." + Option::dir_sep + project->first("MAKEFILE"), 
				QDir::currentDirPath());
    QFile mkwrapf(mkwrap);
    if(mkwrapf.open(IO_WriteOnly | IO_Translate)) {
	debug_msg(1, "pbuilder: Creating file: %s", mkwrap.latin1());
	QTextStream mkwrapt(&mkwrapf);
	writeHeader(mkwrapt);
	const char *cleans = "uiclean mocclean preprocess_clean ";
	mkwrapt << "#This is a makefile wrapper for PROJECT BUILDER\n"
		<< "all:" << "\n\t" 
		<< "cd " << (project->first("QMAKE_ORIG_TARGET") + ".pbproj/ && pbxbuild") << "\n"
		<< "install: all" << "\n\t" 
		<< "cd " << (project->first("QMAKE_ORIG_TARGET") + ".pbproj/ && pbxbuild install") << "\n"
		<< "distclean clean: preprocess_clean" << "\n\t" 
		<< "cd " << (project->first("QMAKE_ORIG_TARGET") + ".pbproj/ && pbxbuild clean") << "\n"
		<< (!did_preprocess ? cleans : "") << ":" << "\n";
	if(did_preprocess) 
	    mkwrapt << cleans << ":" << "\n\t"
		    << "make -f " 
		    << pbx_dir << Option::dir_sep << "qt_preprocess.mak $@" << endl;
    }
    return TRUE;
}

QString
ProjectBuilderMakefileGenerator::fixEnvs(QString file)
{
    QRegExp reg_var("\\$\\((.*)\\)");
    for(int rep = 0; (rep = reg_var.search(file, rep)) != -1; ) {
	if(project->variables()["QMAKE_PBX_VARS"].findIndex(reg_var.cap(1)) == -1)
	    project->variables()["QMAKE_PBX_VARS"].append(reg_var.cap(1));
	rep += reg_var.matchedLength();
    }
    return file;
}

QString
ProjectBuilderMakefileGenerator::fixEnvsList(QString where)
{
    QString ret;
    const QStringList &l = project->variables()[where];
    for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	fixEnvs((*it));
	if(!ret.isEmpty())
	    ret += " ";
	ret += (*it);
    }
    return ret;
}

QString
ProjectBuilderMakefileGenerator::keyFor(QString block)
{
#if 0 //This make this code much easier to debug..
    return block;
#endif

    QString ret;
    if(!keys.contains(block)) {
#if 0
	static unsigned int r = 0;
	ret.sprintf("%024x", ++r);
#else //not really necesary, but makes it look more interesting..
	static struct { unsigned int a1, a2, a3; } r = { 0, 0, 0 };
	if(!r.a1 && !r.a2 && !r.a3) {
	    r.a1 = rand();
	    r.a2 = rand();
	    r.a3 = rand();
	}
	switch(rand() % 3) {
	case 0: ++r.a1; break;
	case 1: ++r.a2; break;
	case 2: ++r.a3; break;
	}
	ret.sprintf("%08x%08x%08x", r.a1, r.a2, r.a3);
#endif
	ret = ret.upper();
	keys.insert(block, ret);
    } else {
	ret = keys[block];
    }
    return ret;
}

bool
ProjectBuilderMakefileGenerator::openOutput(QFile &file) const
{
    if(project->first("TEMPLATE") != "subdirs") {
	QFileInfo fi(file);
	if(fi.extension() != "pbxproj" || file.name().isEmpty()) {
	    QString output = file.name();
	    if(fi.isDir())
		output += QDir::separator();
	    if(fi.extension() != "pbproj") {
		if(file.name().isEmpty() || fi.isDir())
		    output += project->first("TARGET");
		output += QString(".pbproj") + QDir::separator();
	    } else if(output[(int)output.length() - 1] != QDir::separator()) {
		output += QDir::separator();
	    }
	    output += QString("project.pbxproj");
	    file.setName(output);
	}
	bool ret = UnixMakefileGenerator::openOutput(file);
	((ProjectBuilderMakefileGenerator*)this)->pbx_dir = Option::output_dir.section(Option::dir_sep, 0, -1);
	Option::output_dir = pbx_dir.section(Option::dir_sep, 0, -2); 
	return ret;
    }
    return UnixMakefileGenerator::openOutput(file);
}

/* This function is such a hack it is almost pointless, but it
   eliminates the warning message from ProjectBuilder that the project
   file is for an older version. I guess this could be used someday if
   the format of the output is dependant upon the version of
   ProjectBuilder as well.
*/
int
ProjectBuilderMakefileGenerator::pbuilderVersion() const
{
    QString ret;
    if(project->isEmpty("QMAKE_PBUILDER_VERSION")) {
	QString version, version_plist = project->first("QMAKE_PBUILDER_VERSION_PLIST");
	if(version_plist.isEmpty())
	    version_plist = "/Developer/Applications/Project Builder.app/Contents/version.plist";
	else
	    version_plist = version_plist.replace(QRegExp("\""), "");
	QFile version_file(version_plist);
	if(version_file.open(IO_ReadOnly)) {
	    debug_msg(1, "pbuilder: version.plist: Reading file: %s", version_plist.latin1());
	    QTextStream plist(&version_file);

	    bool in_dict = FALSE;
	    QString current_key;
	    QRegExp keyreg("^<key>(.*)</key>$"), stringreg("^<string>(.*)</string>$");
	    while(!plist.eof()) {
		QString line = plist.readLine().stripWhiteSpace();
		if(line == "<dict>")
		    in_dict = TRUE;
		else if(line == "</dict>")
		    in_dict = FALSE;
		else if(in_dict) {
		    if(keyreg.exactMatch(line))
			current_key = keyreg.cap(1);
		    else if(current_key == "CFBundleShortVersionString" && stringreg.exactMatch(line))
			version = stringreg.cap(1);
		}
	    }
	    version_file.close();
	} else debug_msg(1, "pbuilder: version.plist: Failure to open %s", version_plist.latin1());
	if(version.startsWith("2.0"))
	    ret = "38";
	else if(version == "1.1")
	    ret = "34";
    } else {
	ret = project->first("QMAKE_PBUILDER_VERSION");
    }
    if(!ret.isEmpty()) {
	bool ok;
	int int_ret = ret.toInt(&ok);
	if(ok) {
	    debug_msg(1, "pbuilder: version.plist: Got version: %d", int_ret);
	    return int_ret;
	}
    }
    debug_msg(1, "pbuilder: version.plist: Fallback to default version");
    return 34; //my fallback
}

QString
ProjectBuilderMakefileGenerator::reftypeForFile(QString where)
{
    if(QDir::isRelativePath(where))
	return "4"; //relative
    return "0"; //absolute
}
