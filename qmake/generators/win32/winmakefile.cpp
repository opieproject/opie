/****************************************************************************
** $Id: winmakefile.cpp,v 1.1 2002-11-01 00:10:43 kergoth Exp $
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

#include "winmakefile.h"
#include "option.h"
#include "project.h"
#include <qtextstream.h>
#include <qstring.h>
#include <qdict.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdir.h>


Win32MakefileGenerator::Win32MakefileGenerator(QMakeProject *p) : MakefileGenerator(p)
{

}


struct SubDir
{
    QString directory, profile, target, makefile;
};

void
Win32MakefileGenerator::writeSubDirs(QTextStream &t)
{
    QPtrList<SubDir> subdirs;
    {
	QStringList subdirs_in = project->variables()["SUBDIRS"];
	for(QStringList::Iterator it = subdirs_in.begin(); it != subdirs_in.end(); ++it) {
	    QString file = (*it);
	    file = fileFixify(file);
	    SubDir *sd = new SubDir;
	    subdirs.append(sd);
	    sd->makefile = "$(MAKEFILE)";
	    if((*it).right(4) == ".pro") {
		int slsh = file.findRev(Option::dir_sep);
		if(slsh != -1) {
		    sd->directory = file.left(slsh+1);
		    sd->profile = file.mid(slsh+1);
		} else {
		    sd->profile = file;
		}
	    } else {
		sd->directory = file;
	    }
	    while(sd->directory.right(1) == Option::dir_sep)
		sd->directory = sd->directory.left(sd->directory.length() - 1);
	    if(!sd->profile.isEmpty()) {
		QString basename = sd->directory;
		int new_slsh = basename.findRev(Option::dir_sep);
		if(new_slsh != -1)
		    basename = basename.mid(new_slsh+1);
		if(sd->profile != basename + ".pro")
		    sd->makefile += "." + sd->profile.left(sd->profile.length() - 4); //no need for the .pro
	    }
	    sd->target = "sub-" + (*it);
	    sd->target.replace('/', '-');
	    sd->target.replace('.', '_');
	}
    }
    QPtrListIterator<SubDir> it(subdirs);

    if(!project->isEmpty("MAKEFILE"))
	t << "MAKEFILE=	" << var("MAKEFILE") << endl;
    t << "QMAKE =	" << (project->isEmpty("QMAKE_QMAKE") ? QString("qmake") : var("QMAKE_QMAKE")) << endl;
    t << "SUBTARGETS	= ";
    for( it.toFirst(); it.current(); ++it) 
	t << " \\\n\t\t" << it.current()->target;
    t << endl << endl;
    t << "all: qmake_all $(SUBTARGETS)" << endl << endl;

    for( it.toFirst(); it.current(); ++it) {
	bool have_dir = !(*it)->directory.isEmpty();

	//make the makefile
	QString mkfile = (*it)->makefile;
	if(have_dir)
	    mkfile.prepend((*it)->directory + Option::dir_sep);
	t << mkfile << ":";
	if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	    t << " FORCE";
	if(have_dir) 
	    t << "\n\t" << "cd " << (*it)->directory;
	t << "\n\t" << "$(QMAKE) " << (*it)->profile << " " << buildArgs();
	if((*it)->makefile != "$(MAKEFILE)")
	    t << " -o " << (*it)->makefile;
	if(have_dir) {
	    int subLevels = it.current()->directory.contains(Option::dir_sep) + 1;
	    t << "\n\t" << "@cd ..";
	    for(int i = 1; i < subLevels; i++ )
		t << Option::dir_sep << "..";
	}
	t << endl;

	//now actually build
	t << (*it)->target << ": " << mkfile;
	if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	    t << " FORCE";
	if(have_dir) 
	    t << "\n\t" << "cd " << (*it)->directory;
	t << "\n\t" << "$(MAKE)";
	if((*it)->makefile != "$(MAKEFILE)")
	    t << " -f " << (*it)->makefile;
	if(have_dir) {
	    int subLevels = it.current()->directory.contains(Option::dir_sep) + 1;
	    t << "\n\t" << "@cd ..";
	    for(int i = 1; i < subLevels; i++ )
		t << Option::dir_sep << "..";
	}
	t << endl << endl;
    }

    if(project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].findIndex("qmake_all") == -1)
	project->variables()["QMAKE_INTERNAL_QMAKE_DEPS"].append("qmake_all");
    writeMakeQmake(t);

    t << "qmake_all:";
    if ( !subdirs.isEmpty() ) {
	for( it.toFirst(); it.current(); ++it) {
	    QString subdir = (*it)->directory;
	    int subLevels = subdir.contains(Option::dir_sep) + 1;
	    t << "\n\t"
	      << "cd " << subdir << "\n\t";
	    int lastSlash = subdir.findRev(Option::dir_sep);
	    if(lastSlash != -1)
		subdir = subdir.mid( lastSlash + 1 );
	    t << "$(QMAKE) " << subdir << ".pro"
	      << (!project->isEmpty("MAKEFILE") ? QString(" -o ") + var("MAKEFILE") : QString(""))
	      << " " << buildArgs() << "\n\t"
	      << "@cd ..";
	    for(int i = 1; i < subLevels; i++ )
		t << Option::dir_sep << "..";
	}
    } else {
	// Borland make does not like empty an empty command section, so insert
	// a dummy command.
	t << "\n\t" << "@cd .";
    }
    t << endl << endl;

    QString targs[] = { QString("clean"), QString("install"), QString("mocclean"), QString::null };
    for(int x = 0; targs[x] != QString::null; x++) {
        t << targs[x] << ": qmake_all";
	if(targs[x] == "clean")
	    t << varGlue("QMAKE_CLEAN","\n\t-del ","\n\t-del ", "");
	if (!subdirs.isEmpty()) {
	    for( it.toFirst(); it.current(); ++it) {
		int subLevels = (*it)->directory.contains(Option::dir_sep) + 1;
		bool have_dir = !(*it)->directory.isEmpty();
		if(have_dir)
		    t << "\n\t" << "cd " << (*it)->directory;
		QString in_file;
		if((*it)->makefile != "$(MAKEFILE)")
		    in_file = " -f " + (*it)->makefile;
		t << "\n\t" << "$(MAKE) " << in_file << " " << targs[x];
		if(have_dir) {
		    t << "\n\t" << "@cd ..";
		    for(int i = 1; i < subLevels; i++ ) 
			t << Option::dir_sep << "..";
		}
	    }
	} else {
	    // Borland make does not like empty an empty command section, so
	    // insert a dummy command.
	    t << "\n\t" << "@cd .";
	}
	t << endl << endl;
    }

    if(project->variables()["QMAKE_NOFORCE"].isEmpty())
	t << "FORCE:" << endl << endl;
}


int
Win32MakefileGenerator::findHighestVersion(const QString &d, const
					   QString &stem)
{
    if(!QFile::exists(Option::fixPathToLocalOS(d)))
	return -1;
    if(!project->variables()["QMAKE_" + stem.upper() +
	"_VERSION_OVERRIDE"].isEmpty())
	return project->variables()["QMAKE_" + stem.upper() +
	"_VERSION_OVERRIDE"].first().toInt();
    QString bd = d;
    fixEnvVariables(bd);
    QDir dir(bd);
    int biggest=-1;
    QStringList entries = dir.entryList();
    QRegExp regx( "(" + stem + "([0-9]*)).lib", FALSE );
    for(QStringList::Iterator it = entries.begin(); it != entries.end();
    ++it) {
	if(regx.exactMatch((*it)))
	    biggest = QMAX(biggest, (regx.cap(1) == stem ||
	    regx.cap(2).isEmpty()) ? -1 : regx.cap(2).toInt());
    }
    return biggest;
}


bool
Win32MakefileGenerator::findLibraries(const QString &where)
{

    QStringList &l = project->variables()[where];
    QPtrList<MakefileDependDir> dirs;
    dirs.setAutoDelete(TRUE);
    for(QStringList::Iterator it = l.begin(); it != l.end(); ) {
	QString opt = (*it);
        bool remove = FALSE;
        if(opt.startsWith("-L") || opt.startsWith("/L")) {
            QString r = opt.right(opt.length() - 2), l = Option::fixPathToLocalOS(r);
            dirs.append(new MakefileDependDir(r.replace("\"",""),
                                              l.replace("\"","")));
            remove = TRUE;
        } else if(opt.startsWith("-l") || opt.startsWith("/l")) {
            QString lib = opt.right(opt.length() - 2), out;
            if(!lib.isEmpty()) {
                for(MakefileDependDir *mdd = dirs.first(); mdd; mdd = dirs.next() ) {
                    int ver = findHighestVersion(mdd->local_dir, lib);
		    if(ver > 0)
			lib += QString::number(ver);
		    lib += ".lib";
		    if(QFile::exists(mdd->local_dir + Option::dir_sep + lib)) {
			out = mdd->real_dir + Option::dir_sep + lib;
			break;
		    }
                }
            }
            if(out.isEmpty())
                remove = TRUE;
            else
                (*it) = out;
        } else if(!QFile::exists(Option::fixPathToLocalOS(opt))) {
	    QString dir, file = opt;
            int slsh = file.findRev(Option::dir_sep);
            if(slsh != -1) {
                dir = file.left(slsh+1);
                file = file.right(file.length() - slsh - 1);
            }
	    if ( !(project->variables()["QMAKE_QT_DLL"].isEmpty() && (file == "qt.lib" || file == "qt-mt.lib")) ) {
		if(file.endsWith(".lib")) {
		    file = file.left(file.length() - 4);
		    if(!file.at(file.length()-1).isNumber()) {
			int ver = findHighestVersion(dir, file);
			if(ver != -1) {
			    file = QString(dir + file + "%1" + ".lib");
			    if(ver)
				(*it) = file.arg(ver);
			    else
				(*it) = file.arg("");
			}
		    }
		}
	    }
        }
        if(remove)
            it = l.remove(it);
        else
            ++it;
    }
    return TRUE;
}

void
Win32MakefileGenerator::processPrlFiles()
{
    QDict<void> processed;
    QPtrList<MakefileDependDir> libdirs;
    libdirs.setAutoDelete(TRUE);
    {
	QStringList &libpaths = project->variables()["QMAKE_LIBDIR"];
	for(QStringList::Iterator libpathit = libpaths.begin(); libpathit != libpaths.end(); ++libpathit) {
	    QString r = (*libpathit), l = r;
	    fixEnvVariables(l);
	    libdirs.append(new MakefileDependDir(r.replace("\"",""),
						 l.replace("\"","")));
	}
    }
    for(bool ret = FALSE; TRUE; ret = FALSE) {
	//read in any prl files included..
	QStringList l_out;
	QString where = "QMAKE_LIBS";
	if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
	    where = project->first("QMAKE_INTERNAL_PRL_LIBS");
	QStringList &l = project->variables()[where];
	for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
	    QString opt = (*it);
	    if(opt.left(1) == "/") {
		if(opt.left(9) == "/LIBPATH:") {
		    QString r = opt.mid(9), l = r;
		    fixEnvVariables(l);
		    libdirs.append(new MakefileDependDir(r.replace("\"",""),
							 l.replace("\"","")));
		}
	    } else {
		if(!processed[opt]) {
		    if(processPrlFile(opt)) {
			processed.insert(opt, (void*)1);
			ret = TRUE;
		    } else {
			for(MakefileDependDir *mdd = libdirs.first(); mdd; mdd = libdirs.next() ) {
			    QString prl = mdd->local_dir + Option::dir_sep + opt;
			    if(processed[prl]) {
				break;
			    } else if(processPrlFile(prl)) {
				processed.insert(prl, (void*)1);
				ret = TRUE;
				break;
			    }
			}
		    }
		}
	    }
	    if(!opt.isEmpty())
		l_out.append(opt);
	}
	if(ret)
	    l = l_out;
	else
	    break;
    }
}
