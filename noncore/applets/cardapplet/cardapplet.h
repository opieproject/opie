/*
                             This file is part of the Opie Project
             =.             (C) Copyright 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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

#ifndef CARDAPPLET_H
#define CARDAPPLET_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmap.h>

namespace Opie { namespace Core {
class OPcmciaSocket;
class OProcess;
} };

class CardApplet : public QWidget
{
    Q_OBJECT
  public:
    CardApplet( QWidget *parent = 0 );
    ~CardApplet();
    static int position();

  private slots:
    void handleSystemChannel( const QCString&, const QByteArray& );
    void cardMessage( const QCString&, const QByteArray& );
    void userCardAction( int action );
    void popupTimeout();
    void slotExited( Opie::Core::OProcess* proc );
    void slotStdout( Opie::Core::OProcess* proc, char* buf, int len );
    void slotStderr( Opie::Core::OProcess* proc, char* buf, int len );

  protected:
    void paintEvent( QPaintEvent* );
    void mousePressEvent( QMouseEvent * );
    void updatePcmcia();
    void updateMounts( bool showPopup );
    void mountChanged( const QString &device, bool mounted );
    void unmount( const QString &device, bool fusercheck );
    void getProcessInfo( const QString &processes );

  private:
    void configure( Opie::Core::OPcmciaSocket* );
    void execCommand( const QStringList &command );
    void executeAction( Opie::Core::OPcmciaSocket*, const QString& );
    void popUp( QString message, QString icon = QString::null );

  private:
    bool configuring;
    int m_commandOrig;
    QString m_commandStdout;
    QString m_commandStderr;
    QString m_mountpt;
    QPixmap pm;
    QPopupMenu *popupMenu;
    Opie::Core::OProcess *m_process;
    QMap<QString,QString> m_mounts;
    QMap<QString,int> m_cardmounts;
    int m_ejectMode;
    int m_ejectSocket;

};

#endif

