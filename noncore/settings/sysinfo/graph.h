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

#include <qframe.h>
#include <qarray.h>
#include <qstringlist.h>

class GraphData
{
public:
    void clear();
    void addItem( const QString &name, int value );

    const QString &name( int i ) const { return names[i]; }
    int value( int i ) const { return values[i]; }
    unsigned count() const { return values.size(); }

private:
    QStringList names;
    QArray<int> values;
};

class Graph : public QFrame
{
    Q_OBJECT
public:
    Graph( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

    void setData( const GraphData *p ) { data = p; }

protected:
    const GraphData *data;
};

class PieGraph : public Graph
{
    Q_OBJECT
public:
    PieGraph( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

protected:
    virtual void drawContents( QPainter *p );
};

class BarGraph : public Graph
{
    Q_OBJECT
public:
    BarGraph( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

protected:
    virtual void drawContents( QPainter *p );
    void drawSegment( QPainter *p, const QRect &r, const QColor &c );
};

class GraphLegend : public QFrame
{
    Q_OBJECT
public:
    GraphLegend( QWidget *parent = 0, const char *name = 0, WFlags f = 0 );

    void setData( const GraphData *p );
    virtual QSize sizeHint() const;
    void setOrientation(Orientation o);

protected:
    virtual void drawContents( QPainter *p );

private:
    const GraphData *data;
    bool horz;
};
