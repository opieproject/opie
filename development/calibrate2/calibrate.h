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

#include <qwsmouse_qws.h>

#if defined(Q_WS_QWS) || defined(_WS_QWS_)

#include <qdialog.h>
#include <qpixmap.h>

struct calibration {
        int x[5], xfb[5];
        int y[5], yfb[5];
        int a[7];
};

class Calibrate : public QDialog
{
    Q_OBJECT
public:
    Calibrate(QWidget* parent=0, const char * name=0, WFlags=0);
    ~Calibrate();

    void show();
    void hide();

private:
    QPoint fromDevice( const QPoint &p );
    bool performCalculation(void);
    void moveCrosshair( QPoint pt );
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    struct calibration cal;

private:
    void store();
    void reset();
    QPixmap logo;
    QPixmap saveUnder;
    bool showCross;
    QPoint crossPos;
    QPoint penPos;
    int samples;
    int location;
};

#endif // _WS_QWS_
