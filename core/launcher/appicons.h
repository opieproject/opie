/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef APPICONS_H
#define APPICONS_H


#include <qhbox.h>
#include <qintdict.h>
#include <qlabel.h>


class AppIcons : public QHBox
{
    Q_OBJECT

public:
    AppIcons( QWidget *parent );
    void setIcon(int id, const QPixmap&);
    void setToolTip(int id, const QString&);
    void remove(int id);

private slots:
    void receive( const QCString &msg, const QByteArray &data );
    void clicked(const QPoint& relpos, int button, bool dbl);

private:
    QIntDict<QLabel> buttons;
    QLabel* button(int id);
    int findId(QLabel*);
};


#endif
