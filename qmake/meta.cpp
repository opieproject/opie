/****************************************************************************
** 
**
** Implementation of QMakeMetaInfo class.
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of qmake.
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

#include "meta.h"
#include "project.h"
#include "option.h"
#include <qdir.h>

QMap<QString, QMap<QString, QStringList> > QMakeMetaInfo::cache_vars;

QMakeMetaInfo::QMakeMetaInfo()
{
    
}


bool
QMakeMetaInfo::readLib(const QString &lib)
{
    clear();
    QString meta_file = findLib(lib);
    
    if(cache_vars.contains(meta_file)) {
	vars = cache_vars[meta_file];
	return TRUE;
    }

    bool ret = FALSE;
    if(!meta_file.isNull()) {
	if(meta_file.endsWith(Option::pkgcfg_ext)) {
	    if((ret=readPkgCfgFile(meta_file))) 
		meta_type = "pkgcfg";
	} else if(meta_file.endsWith(Option::libtool_ext)) {
	    if((ret=readLibtoolFile(meta_file))) 
		meta_type = "libtool";
	} else if(meta_file.endsWith(Option::prl_ext)) {
	    QMakeProject proj;
	    if(!proj.read(Option::fixPathToLocalOS(meta_file), 
			  QDir::currentDirPath(), QMakeProject::ReadProFile))
		return FALSE;
	    meta_type = "qmake";
	    vars = proj.variables();
	    ret = TRUE;
	} else {
	    warn_msg(WarnLogic, "QMakeMetaInfo: unknown file format for %s", meta_file.latin1());
	}
    }
    if(ret) 
	cache_vars.insert(meta_file, vars);
    return ret;
}


void
QMakeMetaInfo::clear()
{
    vars.clear();
}


QString
QMakeMetaInfo::findLib(const QString &lib)
{
    QString ret = QString::null;
    QString extns[] = { Option::prl_ext, /*Option::pkgcfg_ext, Option::libtool_ext,*/ QString::null };
    for(int extn = 0; !extns[extn].isNull(); extn++) {
	if(lib.endsWith(extns[extn]))
	    ret = QFile::exists(lib) ? lib : QString::null;
    }
    if(ret.isNull()) {
	for(int extn = 0; !extns[extn].isNull(); extn++) {
	    if(QFile::exists(lib + extns[extn])) {
		ret = lib + extns[extn];
		break;
	    }
	}
    }
    if(ret.isNull())
	debug_msg(2, "QMakeMetaInfo: Cannot find info file for %s", lib.latin1());
    else
	debug_msg(2, "QMakeMetaInfo: Found info file %s for %s", ret.latin1(), lib.latin1());
    return ret;
}


bool
QMakeMetaInfo::readLibtoolFile(const QString &f)
{
    /* I can just run the .la through the .pro parser since they are compatible.. */
    QMakeProject proj;
    if(!proj.read(Option::fixPathToLocalOS(f), QDir::currentDirPath(), QMakeProject::ReadProFile))
	return FALSE;
    QString dirf = Option::fixPathToTargetOS(f).section(Option::dir_sep, 0, -2);
    if(dirf == f)
	dirf = "";
    else if(!dirf.isEmpty() && !dirf.endsWith(Option::output_dir))
	dirf += Option::dir_sep;
    QMap<QString, QStringList> &v = proj.variables();
    for(QMap<QString, QStringList>::Iterator it = v.begin(); it != v.end(); ++it) {
	QStringList lst = it.data();
	if(lst.count() == 1 && (lst.first().startsWith("'") || lst.first().startsWith("\"")) &&
	   lst.first().endsWith(QString(lst.first()[0])))
	    lst = lst.first().mid(1, lst.first().length() - 2);
	if(!vars.contains("QMAKE_PRL_TARGET") &&
	   (it.key() == "dlname" || it.key() == "library_names" || it.key() == "old_library")) {
	    QString dir = v["libdir"].first();
	    if((dir.startsWith("'") || dir.startsWith("\"")) && dir.endsWith(QString(dir[0])))
		dir = dir.mid(1, dir.length() - 2);
	    dir = dir.stripWhiteSpace();
	    if(!dir.isEmpty() && !dir.endsWith(Option::dir_sep))
		dir += Option::dir_sep;
	    if(lst.count() == 1)
		lst = QStringList::split(" ", lst.first());
	    for(QStringList::Iterator lst_it = lst.begin(); lst_it != lst.end(); ++lst_it) {
		bool found = FALSE;
		QString dirs[] = { "", dir, dirf, dirf + ".libs" + QDir::separator(), "(term)" };
		for(int i = 0; !found && dirs[i] != "(term)"; i++) {
		    if(QFile::exists(dirs[i] + (*lst_it))) {
			QString targ = dirs[i] + (*lst_it);
			if(QDir::isRelativePath(targ)) 
			    targ.prepend(QDir::currentDirPath() + QDir::separator());
			vars["QMAKE_PRL_TARGET"] << targ;
			found = TRUE;
		    }
		}
		if(found)
 		    break;
	    }
	} else if(it.key() == "dependency_libs") {
	    if(lst.count() == 1) {
		QString dep = lst.first();
		if((dep.startsWith("'") || dep.startsWith("\"")) && dep.endsWith(QString(dep[0])))
		    dep = dep.mid(1, dep.length() - 2);
		lst = QStringList::split(" ", dep.stripWhiteSpace());
	    }
	    QMakeProject *conf = NULL;
	    for(QStringList::Iterator lit = lst.begin(); lit != lst.end(); ++lit) {
		if((*lit).startsWith("-R")) {
		    if(!conf) {
			conf = new QMakeProject;
			conf->read(QMakeProject::ReadAll ^ QMakeProject::ReadProFile);
		    }
		    if(!conf->isEmpty("QMAKE_RPATH"))
			(*lit) = conf->first("QMAKE_RPATH") + (*lit).mid(2);
		}
	    }
	    if(conf)
		delete conf;
	    vars["QMAKE_PRL_LIBS"] += lst;
	}
    }
    return TRUE;
}

bool
QMakeMetaInfo::readPkgCfgFile(const QString &f)
{
    fprintf(stderr, "Must implement reading in pkg-config files (%s)!!!\n", f.latin1());
    return FALSE;
}
