/****************************************************************************
** $Id: pbuilder_pbx.h,v 1.1 2002-11-01 00:10:42 kergoth Exp $
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
#ifndef __PBUILDERMAKE_H__
#define __PBUILDERMAKE_H__

#include "unixmake.h"

class ProjectBuilderMakefileGenerator : public UnixMakefileGenerator
{
    QString pbx_dir;
    int pbuilderVersion() const;
    bool writeMakeParts(QTextStream &);
    bool writeMakefile(QTextStream &);

    QMap<QString, QString> keys;
    QString keyFor(QString file);
    QString fixEnvs(QString file);
    QString fixEnvsList(QString where);
    QString reftypeForFile(QString file);

public:
    ProjectBuilderMakefileGenerator(QMakeProject *p);
    ~ProjectBuilderMakefileGenerator();

    virtual bool openOutput(QFile &) const;
protected:
    virtual bool doDepends() const { return FALSE; } //never necesary
};

inline ProjectBuilderMakefileGenerator::~ProjectBuilderMakefileGenerator()
{ }


#endif /* __PBUILDERMAKE_H__ */
