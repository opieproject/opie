/**********************************************************************
** MGraph
**
** A reusable graph widget
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef MGRAPH_H
#define MGRAPH_H

#include <qframe.h>
#include <qvaluelist.h>

typedef QValueList<int> ValueList;

class MGraph : public QFrame
{
    Q_OBJECT
public:
    MGraph( QWidget *parent = 0, const char *name = 0, WFlags f = 0);
    ~MGraph();

    void addValue( int value, bool followMax = true );
    void setMax( int value ) { max = value; };
    void setMin( int value ) { min = value; };

    virtual void setFrameStyle( int style );

protected:
    virtual void drawContents( QPainter* );
    int min;
    int max;
    ValueList* values;

    QPixmap* background;

private:

};

#endif
