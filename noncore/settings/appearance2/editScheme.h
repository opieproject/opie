/**********************************************************************
** EditScheme
**
** Dialog for editing color scheme
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
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

#ifndef EDITSCHEME_H
#define EDITSCHEME_H

#include <qdialog.h>
#include <qtoolbutton.h>

class QColor;

class EditScheme : public QDialog
{
    Q_OBJECT

public:
    EditScheme( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0,
                int = 0, QString * = 0, QString * = 0 );
    ~EditScheme();

    int maxCount;
    QString * surfaceList;
    QString * colorList;


    QToolButton** colorButtons;

protected slots:
    void changeColor( const QColor& );
};

#endif // EDITSCHEME_H
