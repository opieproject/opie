/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com
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

#ifndef __SCREENSHOT_APPLET_H__
#define __SCREENSHOT_APPLET_H__



#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qguardedptr.h>
#include <qtimer.h>

class QComboBox;
class QCheckBox;
class QSpinBox;
class QPushButton;
//class QImage;

class ScreenshotControl : public QFrame
{
    Q_OBJECT
public:
    ScreenshotControl( QWidget *parent=0, const char *name=0 );

private:
    QPushButton *grabItButton, *scapButton;
    QPixmap snapshot;
    QTimer* grabTimer;
    QCheckBox *saveNamedCheck;
    QString FileNamePath;
    bool setFileName;
    QSpinBox  *delaySpin;
    int buttonPushed;

private slots:
    void slotGrab();
    void slotScap(); 
    void savePixmap();
    void performGrab();
};

class ScreenshotApplet : public QWidget {
public:
    ScreenshotApplet( QWidget *parent = 0, const char *name=0 );
    ~ScreenshotApplet();
    
protected:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );
    
private:
	QPixmap m_icon;
};


#endif // __SCREENSHOT_APPLET_H__

