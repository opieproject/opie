/*
                             This file is part of the Opie Project

              =.             (C) 2004 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
            .=l.             Based on Qtopia 1.7 Brightnessapplet (C) 2003-2004 TrollTech
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this application; see the file COPYING.LIB.
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
#include <qpixmap.h>

class QShowEvent;
class QHideEvent;
class QSlider;

class BrightnessAppletControl : public QFrame
{
  public:
    BrightnessAppletControl( Opie::Ui::OTaskbarApplet* parent, const char* name = 0 );
    ~BrightnessAppletControl();

    QSlider* slider;
};

class BrightnessApplet : public Opie::Ui::OTaskbarApplet
{
  Q_OBJECT

  public:
    BrightnessApplet( QWidget* parent = 0, const char* name = 0 );
    ~BrightnessApplet();

    void writeSystemBrightness( int brightness );
    int readSystemBrightness();
    int calcBrightnessValue();

    static int position();

  public slots:
    void sliderMoved( int value );

  protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void mousePressEvent( QMouseEvent* );

  private:
    BrightnessAppletControl* _control;
    QPixmap _pixmap;
};

#endif

