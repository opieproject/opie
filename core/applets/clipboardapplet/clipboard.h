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
#ifndef __CLIPBOARD_APPLET_H__
#define __CLIPBOARD_APPLET_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qstringlist.h>

class QTimer;

class ClipboardApplet : public QWidget
{
    Q_OBJECT
public:
    ClipboardApplet ( QWidget *parent = 0, const char *name=0 );
    ~ClipboardApplet ( );

protected:
    void mousePressEvent ( QMouseEvent *);
    void paintEvent ( QPaintEvent* );

private slots:
    void action ( int );
	void newData ( );
	void shutdown ( );

private:
    QPopupMenu * m_menu;
    QStringList  m_history;
    bool         m_dirty;
    QString      m_lasttext;
    QTimer *     m_timer;
    
    QPixmap      m_clipboardPixmap;
};


#endif // __CLIPBOARD_APPLET_H__

