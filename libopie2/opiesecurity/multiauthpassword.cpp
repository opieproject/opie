/**
 * \file multiauthpassword.cpp
 * \brief Password Dialog dropin.
 * \author Clément Séveillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


/* OPIE */
#include <opie2/multiauthcommon.h>
#include <opie2/multiauthmainwindow.h>
#include <qpe/config.h>
#include <qpe/qlibrary.h>
#include <qpe/qcom.h>

/* QT */
#include <qapplication.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qdir.h>

#include "multiauthpassword.h"

namespace Opie {
namespace Security {


/**
 * If the users requires authentication... #fixme
 *
 * @todo fix up at_poweron attribute
 */
bool MultiauthPassword::needToAuthenticate(bool at_poweron)
{
    Config cfg("Security");
    cfg.setGroup("Misc");
    if ( !at_poweron && cfg.readBoolEntry("onStart", false) )
        return true;
    else if ( at_poweron && cfg.readBoolEntry("onResume", false) )
        return true;
    else
        return false;
}


/**
 * \brief Require user authentication to unlock and continue
 *
 * This method will check if you require authentication
 * and then will lock the screen and require a succesfull
 * authentication.
 * Authenticate may enter the event loop and only returns
 * if the user sucesfully authenticated to the system.
 *
 * @return only if succesfully authenticated
 */
void MultiauthPassword::authenticate(bool at_poweron)
{
    if ( ! needToAuthenticate(at_poweron) )
        return;

    /* Constructs the main window, which displays messages and blocks
     * access to the desktop
     */
    MultiauthMainWindow win;

    // resize the QDialog object so it fills all the screen
    QRect desk = qApp->desktop()->geometry();
    win.setGeometry( 0, 0, desk.width(), desk.height() );

    // the authentication has already succeeded (without win interactions)
    if ( win.isAlreadyDone() )
        return;

    win.exec();
}

}
}
