/*
                             This file is part of the OPIE Project
               =.            Copyright (c)  2002 Trolltech AS <info@trolltech.com>
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __PREVIEW_H__
#define __PREVIEW_H__

#include <qwidget.h>

#include <qpe/windowdecorationinterface.h>

class QVBox;
class QPopupMenu;
class SampleText;

class SampleWindow : public QWidget
{
    Q_OBJECT
public:
    SampleWindow( QWidget *parent );

    QSize sizeHint() const;

    virtual void setFont( const QFont &f );

	void setStyle2 ( QStyle *sty );
    void setDecoration( WindowDecorationInterface *i );
    void setPalette ( const QPalette & );

    virtual void paintEvent( QPaintEvent * );

    void init();

    virtual bool eventFilter( QObject *, QEvent *e );
    virtual void paletteChange( const QPalette &old );
    virtual void resizeEvent( QResizeEvent *re );

public slots:
    void fixGeometry();

protected:
    WindowDecorationInterface *iface;
    WindowDecorationInterface::WindowData wd;
    QVBox *container;
    QPopupMenu *popup;
    int th;
    int tb;
    int lb;
    int rb;
    int bb;
};

#endif
