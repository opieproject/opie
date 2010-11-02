/**
 * \file multiauthpassword.cpp
 * \brief Password Dialog dropin.
 * \author Clément Séveillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@lists.sourceforge.net>
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

#ifndef QT_NO_COP
#include <qtopia/qcopenvelope_qws.h>
#endif

#include "multiauthpassword.h"

namespace Opie {
namespace Security {

static bool authLock = false;

/**
 * Tells if the users requires authentication (used internally to
 * know whether to repaint the screen on resume)
 *
 * \param at_poweron true if we are booting Opie, false if we are resuming it
 * \return true if authenticate() launched right now would trigger an authentication
 */
bool MultiauthPassword::needToAuthenticate(bool at_poweron)
{
    Config cfg("Security");
    cfg.setGroup("Misc");
    if ( at_poweron && cfg.readBoolEntry("onStart", false) )
        return true;
    else if ( !at_poweron && cfg.readBoolEntry("onResume", false) )
        return true;
    else
        return false;
}

/**
 * Determines if the authentication system is currently shown
 *
 * \return true if the authentication system is showing, false otherwise
 */
bool MultiauthPassword::isAuthenticating()
{
    return authLock;
}

/**
 * \brief Require (if configured so) user authentication to unlock and continue
 *
 * This method will check if you require authentication
 * and then will lock the screen and ask for a successful
 * authentication (explaining what it does or not, depending
 * on your local configuration).
 * It may go into an event loop, but anyhow it will only end
 * when the user has successfully authenticated to the system.
 */
void MultiauthPassword::authenticate(int lockMode)
{
    /**
     * \par Conditions
     *
     * If lockMode is an If, it's conditional:
     * \li IfPowerOn will not trigger an authentication if
     *   onStart is set to false in Security.conf,
     * \li IfResume will not trigger an authentication if
     *   onResume is set to false in Security.conf.
     */
    if ( (lockMode == IfPowerOn) || (lockMode == IfResume) )
    {
        Config cfg("Security");
        cfg.setGroup("Misc");
        if ( (
              (lockMode == IfPowerOn) && !cfg.readBoolEntry("onStart", false)
            ) || (
              (lockMode == IfResume) && !cfg.readBoolEntry("onResume", false)
            ) )
            return;
    }

    authLock = true;

    /**
     * \li TestNow will ensure that the authentication window will let
     * people escape through the last screen (which they can reach skipping
     * all the authentication steps)
     * \li LockNow will always go on with the authentication, and won't let
     * people escape.
     */
    bool allowByPass = false;

    if (lockMode == TestNow)
        allowByPass = true;

    /* Constructs the main window, which displays messages and blocks
     * access to the desktop
     */
    MultiauthMainWindow win(allowByPass);

    // resize the QDialog object so it fills all the screen
    QRect desk = qApp->desktop()->geometry();
    win.setGeometry( 0, 0, desk.width(), desk.height() );

    // Check if the authentication has already succeeded (without
    // win interactions); if not, then show the window
    if ( !win.isAlreadyDone() ) {
        win.exec();
    }

#ifndef QT_NO_COP
    if (lockMode != TestNow)
        QCopEnvelope e( "QPE/Desktop", "unlocked()" );
#endif

    authLock = false;
}

}
}
