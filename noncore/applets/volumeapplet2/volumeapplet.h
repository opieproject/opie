/*
                             This file is part of the Opie Project

              =.             (C) 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef VOLUMEAPPLET_H
#define VOLUMEAPPLET_H

#include <opie2/otaskbarapplet.h>
#include <opie2/osoundsystem.h>
#include <qframe.h>
#include <qstring.h>
#include <qvbox.h>
#include <qpixmap.h>
//using namespace Opie::MM;

namespace Opie
{
    namespace Ui { class OLedBox; }
//    namespace MM { class OMixerInterface; }
}
class QLabel;
class QSlider;
class QShowEvent;
class QHideEvent;
class QGridLayout;

class Channel : public QVBox
{
  public:
    Channel( Opie::MM::OMixerInterface* mixer, QWidget* parent, const char* name );
    virtual ~Channel();

  // public slots:
    // virtual void clicked();

  private:
    QLabel* _name;
    QSlider* _volume;
    Opie::Ui::OLedBox* _mute;
    Opie::MM::OMixerInterface* _mixer;
};

class VolumeAppletControl : public QFrame
{
  public:
    VolumeAppletControl( Opie::Ui::OTaskbarApplet* parent, const char* name = 0 );
    ~VolumeAppletControl();
   bool volMuted() const;
   int volPercent() const;

   int  m_vol_percent;
   bool m_vol_muted;

    Opie::MM::OMixerInterface* mixer;

    virtual QSize sizeHint() const;

  protected:
    virtual void showEvent( QShowEvent* );
    virtual void hideEvent( QHideEvent* );
    void build();

  private:

    QGridLayout* l;
};


class VolumeApplet : public Opie::Ui::OTaskbarApplet
{
  public:
    VolumeApplet( QWidget* parent = 0, const char* name = 0 );
    ~VolumeApplet();

    static int position();
  protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void mousePressEvent( QMouseEvent* );

  private:
    VolumeAppletControl* _control;
    QPixmap _pixmap;
};

#endif

