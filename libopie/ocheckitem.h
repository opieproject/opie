/**********************************************************************
** Copyright (C) 2002 Stefan Eilers (se, eilers.stefan@epost.de)
**
** This file may be distributed and/or modified under the terms of the
** GNU Library General Public License version 2 as published by the
**  Free Software Foundation and appearing in the file LICENSE.GPL 
** included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**********************************************************************/
#include <qtable.h>

#ifndef CHECKITEM_H__
#define CHECKITEM_H__

class OCheckItem : public QTableItem
{
public:
    enum Size { BoxSize = 10 };
    OCheckItem( QTable *t, const QString &sortkey );

    virtual void setChecked( bool b );
    virtual void toggle();
    bool isChecked() const;
    void setKey( const QString &key ) { m_sortKey = key; }
    virtual QString key() const;

    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );

    //static const int BoxSize = 10;

private:
    class OCheckItemPrivate;
    OCheckItemPrivate *d;
    bool m_checked: 1;
    QString m_sortKey;

};

#endif
