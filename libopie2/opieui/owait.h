/*
† † † † † † † †              This file is part of the Opie Project
† † † † † † †                Copyright (C) 2003 Maximilian Reiss <harlekin@handhelds.org>
              =.
            .=l.
† † † † † †.>+-=
†_;:, † † .> † †:=|.         This program is free software; you can
.> <`_, † > †. † <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- † :           the terms of the GNU Library General Public
.="- .-=="i, † † .._         License as published by the Free Software
†- . † .-<_> † † .<>         Foundation; either version 2 of the License,
† † †._= =} † † † :          or (at your option) any later version.
† † .%`+i> † † † _;_.
† † .i_,=:_. † † †-<s.       This program is distributed in the hope that
† † †+ †. †-:. † † † =       it will be useful,  but WITHOUT ANY WARRANTY;
† † : .. † †.:, † † . . .    without even the implied warranty of
† † =_ † † † †+ † † =;=|`    MERCHANTABILITY or FITNESS FOR A
† _.=:. † † † : † †:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= † † † = † † † ;      Library General Public License for more
++= † -. † † .` † † .:       details.
†: † † = †...= . :.=-
†-. † .:....=;==+<;          You should have received a copy of the GNU
† -_. . . † )=. †=           Library General Public License along with
† † -- † † † †:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef OWAIT_H
#define OWAIT_H

/* QT */
#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>


namespace Opie {
namespace Ui   {
/**
 * This class displays a animated waiting icon in the middle of the screen.
 *
 * @short modal hour glass dialog
 * @see QDialog
 * @author Maximilian Reiﬂ
 */
class OWait : public QDialog
{
    Q_OBJECT

  public:
    OWait( QWidget *parent = 0, const char* name = 0, bool dispIcon = TRUE );
    ~OWait();

    /**
     * reimplemented for control reasons
     */
    void show();

    /**
     * Set the time before the icon will be automaticly hidden
     * The timer will be started once the widget will be shown.
     * @param length - time in seconds
     */
    void setTimerLength( int length );

  public slots:
    /**
     * reimplemented for control reasons
     */
    void hide();

  private:
    void timerEvent( QTimerEvent * );
    void paintEvent( QPaintEvent * );

    QPixmap m_pix;
    QLabel *m_lb;
    QTimer *m_waitTimer;
    int m_timerLength;
    int m_aniSize;
    class Private;
    Private *d;
};
}
}
#endif
