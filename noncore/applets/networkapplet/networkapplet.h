/*
                             This file is part of the Opie Project

              =.             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
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

#ifndef NETWORKAPPLET_H
#define NETWORKAPPLET_H

#include <opie2/otaskbarapplet.h>
#include <qframe.h>
#include <qstring.h>
#include <qtoolbutton.h>
#include <qlineedit.h>

class ONetworkInterface;
class QShowEvent;
class QHideEvent;
class QVBoxLayout;

class IfaceUpDownButton : public QToolButton
{
  Q_OBJECT

  public:
    IfaceUpDownButton( QWidget* parent, const char* name );
    virtual ~IfaceUpDownButton();

  public slots:
    virtual void clicked();

  private:
    ONetworkInterface* _iface;
};


class IfaceIPAddress : public QLineEdit
{
  Q_OBJECT

  public:
    IfaceIPAddress( QWidget* parent, const char* name );
    virtual ~IfaceIPAddress();

  public slots:
    virtual void returnPressed();

  private:
    ONetworkInterface* _iface;
};

class NetworkAppletControl : public QFrame
{
  public:
    NetworkAppletControl( OTaskbarApplet* parent, const char* name = 0 );
    ~NetworkAppletControl();

    virtual QSize sizeHint() const;

  protected:
    virtual void showEvent( QShowEvent* );
    virtual void hideEvent( QHideEvent* );
    QString guessDevice( ONetworkInterface* iface );
    void build();

  private:
    QVBoxLayout* l;

};


class NetworkApplet : public OTaskbarApplet
{
  public:
    NetworkApplet( QWidget* parent = 0, const char* name = 0 );
    ~NetworkApplet();

    static int position();
  protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void mousePressEvent( QMouseEvent* );

  private:
    NetworkAppletControl* _control;
};

#endif

