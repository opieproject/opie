/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
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

#ifndef HAZE_OMODAL_HELPER_H
#define HAZE_OMODAL_HELPER_H

#include <qwidget.h>

class OModalHelperSignal : public QObject {
    Q_OBJECT
public:
    OModalHelperSignal();

signals:
    done( int status,  int transaction );
    accpeted( int transaction );
    rejected( int transaction );
};

/**
 * Modality sucks! ;) But it is easy to work with
 * do exec() on a dialog and you know everything is funky.
 * You only need to have one Dialog loaded and so on.
 * This class helps you to work like with modality and help
 * you to keep things in sync
 * It's a template class but it sends signals once one Item is ready
 * the signals contains the status and id of the item and then you
 * can fetch it.
 * Also if you edit an record external you can tell this class and it'll
 * call the merge() function of your widget to maybe merge in these changes.
 * It also supports multiple modes. Either it can create new dialogs
 * for each item or it can queue them depending on your usage. But it is
 * so smart that if only one item is shown that the queue bar is not shown
 * See the example for simple usage.
 *
 * @short helps to live without modaility
 * @author hOlgAr
 * @version 0.01
 */
template<class Dialog, class Record>
class OModalHelper{
public:
    OModalHelper(QObject* parnet );

};

#endif
