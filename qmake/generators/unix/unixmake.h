/****************************************************************************
** 
**
** Definition of UnixMakefileGenerator class.
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

#ifndef __UNIXMAKE_H__
#define __UNIXMAKE_H__

#include "makefile.h"

class UnixMakefileGenerator : public MakefileGenerator
{
    bool init_flag, include_deps;
    bool writeMakefile(QTextStream &);
    void writeExtraVariables(QTextStream &);
    QString libtoolFileName();
    void writeLibtoolFile();     // for libtool
    QString pkgConfigPrefix() const;
    QString pkgConfigFileName();
    QString pkgConfigFixPath(QString) const;
    void writePkgConfigFile();   // for pkg-config
    QStringList combineSetLFlags(const QStringList &list1, const QStringList &list2);
    void writePrlFile(QTextStream &);

public:
    UnixMakefileGenerator(QMakeProject *p);
    ~UnixMakefileGenerator();

protected:
    virtual bool doPrecompiledHeaders() const { return project->isActiveConfig("precompile_header"); }
    virtual bool doDepends() const { return !include_deps && MakefileGenerator::doDepends(); }
    virtual QString defaultInstall(const QString &);
    virtual void processPrlVariable(const QString &, const QStringList &);
    virtual void processPrlFiles();

    virtual bool findLibraries();
    virtual QString findDependency(const QString &);
    virtual QStringList &findDependencies(const QString &);
    virtual void init();

    void writeMakeParts(QTextStream &);
    void writeSubdirs(QTextStream &, bool=TRUE);
    
private:
    void init2();
};

inline UnixMakefileGenerator::~UnixMakefileGenerator()
{ }


#endif /* __UNIXMAKE_H__ */
