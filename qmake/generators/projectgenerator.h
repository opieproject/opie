/****************************************************************************
** 
**
** Definition of ProjectGenerator class.
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
#ifndef __PROJECTGENERATOR_H__
#define __PROJECTGENERATOR_H__

#include "makefile.h"

class ProjectGenerator : public MakefileGenerator
{
    bool init_flag;
    bool addFile(QString);
    bool addConfig(const QString &, bool add=TRUE);
    QString getWritableVar(const QString &, bool fixPath=TRUE);
protected:
    virtual void init();
    virtual bool writeMakefile(QTextStream &);
public:
    ProjectGenerator(QMakeProject *p);
    ~ProjectGenerator();
    virtual bool openOutput(QFile &) const;
};

inline ProjectGenerator::~ProjectGenerator()
{ }


#endif /* __PROJECTGENERATOR_H__ */
