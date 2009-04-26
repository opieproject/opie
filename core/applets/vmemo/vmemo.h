/*
                             This file is part of the Opie Project
              =.             Copyright 2009 Team Opie <opie@handhelds.org>
            .=l.             Copyright 2002 Jeremy Cowgar <jc@cowgar.com>
           .>+-=             Copyright 2002 and 2003 L.J.Potter <ljp@llornkcor.com>
 _;:,     .>    :=|.         
.> <`_,   >  .   <=          This program is free software; you can
:`=1 )Y*s>-.--   :           redistribute it and/or  modify it under
.="- .-=="i,     .._         the terms of the GNU Library General Public
 - .   .-<_>     .<>         License as published by the Free Software
     ._= =}       :          Foundation; version 2 of the License,
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef __VMEMO_H__
#define __VMEMO_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qlabel.h>
#include <qtimer.h>

#include <opie2/owavrecorder.h>

using namespace Opie::MM;

class VMemo : public QWidget, public OWavRecorderCallback
{
  Q_OBJECT

public:
    VMemo( QWidget *parent, const char *name = NULL);
    ~VMemo();
    static int position();
    QWidget *m_statusWidget;
    QLabel *m_timeLabel;
    QTimer *t_timer;
    bool usingIcon;

public slots:
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void receive( const QCString &msg, const QByteArray &data );
    bool startRecording();
    void stopRecording();
    void timerBreak();

private:
    void recorderCallback(const char *buffer, const int bytes, const int totalbytes, bool &stopflag);

private:
    QPixmap m_pixnormal;
    QPixmap m_pixrecording;
    OWavRecorder m_recorder;
    OWavFileParameters m_fileparams;
    int m_maxseconds;
    int m_elapsedseconds;
    bool m_useAlerts;
    void paintEvent( QPaintEvent* );
    int setToggleButton(int);
    void readSettings();
    void setupStatusWidget( QString );
    QCopChannel *myChannel;
    bool recording;
};

#endif // __VMEMO_H__

