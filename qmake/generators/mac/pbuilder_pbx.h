/****************************************************************************
** 
**
** Definition of ProjectBuilderMakefileGenerator class.
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

#ifndef __PBUILDER_PBX_H__
#define __PBUILDER_PBX_H__

#include "unixmake.h"

class ProjectBuilderMakefileGenerator : public UnixMakefileGenerator
{
    QString pbx_dir;
    int pbuilderVersion() const;
    bool writeSubdirs(QTextStream &, bool);
    bool writeMakeParts(QTextStream &);
    bool writeMakefile(QTextStream &);

    QString pbxbuild();
    QMap<QString, QString> keys;
    QString keyFor(const QString &file);
    QString fixQuotes(const QString &val);
    QString fixEnvs(const QString &file);
    QString fixEnvsList(const QString &where);
    int     reftypeForFile(const QString &where);
    QString projectSuffix() const;

    enum IDE_TYPE { MAC_XCODE, MAC_PBUILDER };
    IDE_TYPE ideType() const;

public:
    ProjectBuilderMakefileGenerator(QMakeProject *p);
    ~ProjectBuilderMakefileGenerator();

    virtual bool openOutput(QFile &) const;
protected:
    bool doPrecompiledHeaders() const { return FALSE; }
    virtual bool doDepends() const { return FALSE; } //never necesary
};

inline ProjectBuilderMakefileGenerator::~ProjectBuilderMakefileGenerator()
{ }


#endif /* __PBUILDER_PBX_H__ */
