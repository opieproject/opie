/**********************************************************************
** Copyright (C) 2002 Stefan Eilers (se, eilers.stefan@epost.de
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2 as published by the Free Software
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
#include <qtable.h>

#ifndef __CHECKITEM_H__
#define __CHECKITEM_H__

class OCheckItem : public QTableItem
{
public:
    OCheckItem( QTable *t, const QString &sortkey );

    void setChecked( bool b );
    void toggle();
    bool isChecked() const;
    void setKey( const QString &key ) { sortKey = key; }
    QString key() const;

    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );

    static const int BoxSize = 10;

private:
    bool checked;
    QString sortKey;

};

#endif
