/*
                             This file is part of the OPIE Project
                             Copyright (c) 2004 Andreas Richter <ar@handhelds.org>
                             Copyright (c) 2004 Holger Hans Peter Freyther <freyther@handhelds.org>
               =.
             .=l.
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#include <qtopia/qpeapplication.h>

#ifdef Q_WS_QWS
extern Q_EXPORT QRect qt_maxWindowRect;
#endif

void QPEApplication::showDialog( QDialog* d, bool nomax )
{
    showWidget( d, nomax );
}

int QPEApplication::execDialog( QDialog* d, bool nomax )
{
    showDialog( d, nomax );
    return d->exec();
}

void QPEApplication::showWidget( QWidget* wg, bool nomax ) {
    if ( wg->isVisible() ) {
        wg->show();
        return;
    }

#ifdef OPIE_NO_WINDOWED
    Q_UNUSED( nomax )
    if ( TRUE ) {
#else
    if ( !nomax
         && ( qApp->desktop()->width() <= 320 ) ){
#endif
        wg->showMaximized();
    } else {
#ifdef Q_WS_QWS
        QSize desk = QSize( qApp->desktop()->width(), qApp->desktop()->height() );
#else
        QSize desk = QSize( qt_maxWindowRect.width(), qt_maxWindowRect.height() );
#endif

        QSize sh = wg->sizeHint();
        int w = QMAX( sh.width(), wg->width() );
        int h = QMAX( sh.height(), wg->height() );
//              desktop                              widget-frame                      taskbar
        w = QMIN( w, ( desk.width() - ( wg->frameGeometry().width() - wg->geometry().width() ) - 25 ) );
        h = QMIN( h, ( desk.height() - ( wg->frameGeometry().height() - wg->geometry().height() ) - 25 ) );
        wg->resize( w, h );
        wg->show();
    }
}
