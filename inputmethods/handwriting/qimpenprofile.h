/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QIMPENPROFILE_H_
#define QIMPENPROFILE_H_

#include "qimpenchar.h"

class QIMPenProfile
{
public:
    QIMPenProfile( const QString &fn );

    const QString &name() const { return pname; }
    const QString &description() const { return pdesc; }

    enum Style { ToggleCases, BothCases };
    Style style() const { return pstyle; }
    void setStyle( Style s );

    bool canSelectStyle() const { return tstyle; }

    int multiStrokeTimeout() const { return msTimeout; }
    void setMultiStrokeTimeout( int t );

    bool matchWords() const { return wordMatch; }

    QIMPenCharSet *uppercase();
    QIMPenCharSet *lowercase();
    QIMPenCharSet *numeric();
    QIMPenCharSet *punctuation();
    QIMPenCharSet *symbol();
    QIMPenCharSet *shortcut();
    QIMPenCharSet *find( QIMPenCharSet::Type t );

    QIMPenCharSetList &charSets();

private:
    QString userConfig();
    void loadData();

private:
    QIMPenCharSetList sets;
    QString filename;
    QString pname;
    QString pdesc;
    Style pstyle;
    bool tstyle;
    int msTimeout;
    bool wordMatch;
};

#endif
