/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __OPIETOOTH2APPLET_H__
#define __OPIETOOTH2APPLET_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qtimer.h>

namespace Opietooth2 {

class OTGateway;

class MessagePanel : public QLabel {

    Q_OBJECT

public:

    MessagePanel( const QString & Msg,
                  QWidget* parent = 0, 
                  const char*  name=0);
    
    void popup( QWidget* parent = 0);

protected:

    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void closeEvent( QCloseEvent * );
    
private:

    QWidget* popupParent;
    int moves;

};

class Opietooth2Applet : public QWidget {

    Q_OBJECT

public:

    Opietooth2Applet( QWidget *parent = 0, const char *name=0 );
    ~Opietooth2Applet();

public slots:

    // sent when device changes state
    void SLOT_StateChange( bool );
    void SLOT_Error( const QString & );

private:

    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );

    void launchManager();

private:

    OTGateway * OT;
    QPixmap OnP;
    QPixmap OffP;

};

};


#endif

