/**********************************************************************
** Copyright (C) 2004 Anton Kachalov mouse@altlinux.ru
**  All rights reserved.
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
#ifndef __MULTIKEY_H__
#define __MULTIKEY_H__

#include <qlabel.h>
#include <qstringlist.h>
#include <qpopupmenu.h>
#include <qcopchannel_qws.h>

class Multikey: public QLabel
{
    Q_OBJECT
public:
    Multikey(QWidget *parent);
    static int position();

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

public slots:
    void message(const QCString &message, const QByteArray &data);

protected:
    QStringList sw_maps;
    QStringList labels;
    QPopupMenu popupMenu;
    QString current;
    uint lang;
};

#endif /* __MULTIKEY_H__ */
