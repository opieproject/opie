/****************************************************************************
** 
**
** Definition of QMakeProject class.
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

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <qstringlist.h>
#include <qstring.h>
#include <qmap.h>

class QMakeProperty;

class QMakeProject
{
    enum TestStatus { TestNone, TestFound, TestSeek } test_status;
    int scope_block, scope_flag;

    QString pfile, cfile;
    QMakeProperty *prop;
    void reset();
    QMap<QString, QStringList> vars, base_vars, cache;
    bool parse(const QString &text, QMap<QString, QStringList> &place);
    bool doProjectTest(const QString &func, const QString &params, QMap<QString, QStringList> &place);
    bool doProjectTest(const QString &func, QStringList args, QMap<QString, QStringList> &place);
    bool doProjectCheckReqs(const QStringList &deps, QMap<QString, QStringList> &place);
    QString doVariableReplace(QString &str, const QMap<QString, QStringList> &place);

public:
    QMakeProject();
    QMakeProject(QMakeProperty *);

    enum { ReadCache=0x01, ReadConf=0x02, ReadCmdLine=0x04, ReadProFile=0x08, ReadPostFiles=0x10, ReadAll=0xFF };
    bool read(const QString &project, const QString &pwd, uchar cmd=ReadAll);
    bool read(uchar cmd=ReadAll);

    QString projectFile();
    QString configFile();

    bool isEmpty(const QString &v);
    QStringList &values(const QString &v);
    QString first(const QString &v);
    QMap<QString, QStringList> &variables();
    bool isActiveConfig(const QString &x, bool regex=FALSE, QMap<QString, QStringList> *place=NULL);

protected:
    friend class MakefileGenerator;
    bool read(const QString &file, QMap<QString, QStringList> &place);

};

inline QString QMakeProject::projectFile()
{
#if defined(Q_CC_SUN) && (__SUNPRO_CC == 0x500) || defined(Q_CC_HP)
    // workaround for Sun WorkShop 5.0 bug fixed in Forte 6
    if (pfile == "-")
	return QString("(stdin)");
    else
	return pfile;
#else
    return pfile == "-" ? QString("(stdin)") : pfile;
#endif
}

inline QString QMakeProject::configFile()
{ return cfile; }

inline bool QMakeProject::isEmpty(const QString &v)
{ return !vars.contains(v) || vars[v].isEmpty(); }

inline QStringList &QMakeProject::values(const QString &v)
{ return vars[v]; }

inline QString QMakeProject::first(const QString &v)
{
#if defined(Q_CC_SUN) && (__SUNPRO_CC == 0x500) || defined(Q_CC_HP)
    // workaround for Sun WorkShop 5.0 bug fixed in Forte 6
    if (isEmpty(v))
	return QString("");
    else
	return vars[v].first();
#else
    return isEmpty(v) ? QString("") : vars[v].first();
#endif
}

inline QMap<QString, QStringList> &QMakeProject::variables()
{ return vars; }

#endif /* __PROJECT_H__ */
