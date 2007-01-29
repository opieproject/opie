/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
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

#include "loginapplication.h"
#include "loginwindowimpl.h"
#include "calibrate.h"

/* OPIE */
#include <opie2/odevice.h>
#include <qpe/qpestyle.h>
#include <qpe/power.h>
#include <qpe/config.h>

/* QT */
#include <qwindowsystem_qws.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qfile.h>

/* STD */
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>

using namespace Opie::Core;

int login_main ( int argc, char **argv, pid_t ppid );
void sigterm ( int sig );
void sigint ( int sig );
void exit_closelog ( );

int main ( int argc, char **argv )
{
    int userExited = 0;
    pid_t ppid = ::getpid ( );

    if ( ::geteuid ( ) != 0 ) {
        ::fprintf ( stderr, "%s can only be executed by root. (or chmod +s)\n", argv [0] );
        return 1;
    }
    /*!
     * @bug
     * Qte does not really like being set UID root. This is
     * largely because we do almost everything on config files
     * in root context. So if you even want to use opie-login
     * you are in for a world of hurt unless someone at least
     * scrubs the settings area and the PIM apps to make sure that
     * they are covered regarding perms and users.
     */
    if ( ::getuid ( ) != 0 )
        ::setuid ( 0 );

    ::setpgid ( 0, 0 );
    ::setsid ( );

    ::signal ( SIGTERM, sigterm );
    ::signal ( SIGINT, sigterm );

    ::openlog ( "opie-login", LOG_CONS, LOG_AUTHPRIV );
    ::atexit ( exit_closelog );

    const char* autolog = 0;
    Config c( "opie-login" );
    c.setGroup( "autologin" );
    QString entry = c.readEntry( "user", "" );
    if ( !entry.isEmpty() )
        autolog = ::strdup( (const char*) entry );
        
    while ( true ) {
        pid_t child = ::fork ( );

        if ( child < 0 ) {
            ::syslog ( LOG_ERR, "Could not fork GUI process\n" );
            break;
        }
        else if ( child > 0 ) {
            int status = 0;
            time_t started = ::time ( 0 );

            while ( ::waitpid ( child, &status, 0 ) < 0 )
                ;

            LoginApplication::logout ( );

            if (( ::time ( 0 ) - started ) < 3 ) {
                if ( autolog ) {
                    ::syslog ( LOG_ERR, "Respawning too fast -- disabling auto-login\n" );
                    autolog = 0;
                }
                else {
                    ::syslog ( LOG_ERR, "Respawning too fast -- going down\n" );
                    break;
                }
            }
            int killedbysig = 0;
            userExited=0;
            if (WIFEXITED(status)!=0 ) {
                if (WEXITSTATUS(status)==137)  {
                    userExited=1;
                }
            }

            if ( WIFSIGNALED( status )) {
                switch ( WTERMSIG( status )) {
                    case SIGTERM:
                    case SIGINT :
                    case SIGKILL:
                        break;

                    default     :
                        killedbysig = WTERMSIG( status );
                        break;
                }
            }
            if ( killedbysig ) {  // qpe was killed by an uncaught signal
                qApp = 0;

                ::syslog ( LOG_ERR, "Opie was killed by a signal #%d", killedbysig );

                QWSServer::setDesktopBackground ( QImage ( ));
                QApplication *app = new QApplication ( argc, argv, QApplication::GuiServer );
                app-> setFont ( QFont ( "Helvetica", 10 ));
                app-> setStyle ( new QPEStyle ( ));

#ifndef __UCLIBC__
                const char *sig = ::sys_siglist[killedbysig];
#else
                const char *sig = ::strsignal ( killedbysig );
#endif
                QLabel *l = new QLabel ( 0, "sig", Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool );
                l-> setText ( LoginWindowImpl::tr( "Opie was terminated\nby an uncaught signal\n(%1)\n" ). arg ( sig ));
                l-> setAlignment ( Qt::AlignCenter );
                l-> move ( 0, 0 );
                l-> resize ( app-> desktop ( )-> width ( ), app-> desktop ( )-> height ( ));
                l-> show ( );
                QTimer::singleShot ( 3000, app, SLOT( quit()));
                app-> exec ( );
                delete app;
                qApp = 0;
            }
        }
        else {
            if ( !autolog ) {
                QString confFile=QPEApplication::qpeDir() + "etc/opie-login.conf";
                Config cfg ( confFile, Config::File );
                cfg. setGroup ( "General" );
                QString user = cfg. readEntry ( "AutoLogin" );

                if ( !user. isEmpty ( ))
                    autolog = ::strdup ( user. latin1 ( ));
            }

            if ( autolog && !userExited ) {
                QWSServer::setDesktopBackground( QImage() );
                ODevice::inst()->setDisplayStatus( true );
                LoginApplication *app = new LoginApplication ( argc, argv, ppid );
                LoginApplication::setLoginAs( autolog );

                if ( LoginApplication::changeIdentity ( ))
                    ::exit ( LoginApplication::login ( ));
                else
                    ::exit ( 0 );
            }
            else  {
                ::exit ( login_main ( argc, argv, ppid ));
            }
        }
    }
    return 0;
}

void sigterm ( int /*sig*/ )
{
    ::exit ( 0 );
}


void exit_closelog ( )
{
    ::closelog ( );
}


class LoginScreenSaver : public QWSScreenSaver
{
    public:
        LoginScreenSaver ( )
        {
            m_lcd_status = true;

            m_backlight_bright = -1;
            m_backlight_forcedoff = false;

            // Make sure the LCD is in fact on, (if opie was killed while the LCD is off it would still be off)
            ODevice::inst ( )-> setDisplayStatus ( true );
        }
        void restore()
        {
            if ( !m_lcd_status ) // We must have turned it off
                ODevice::inst ( ) -> setDisplayStatus ( true );

            setBacklight ( -3 );
        }
        bool save( int level )
        {
            switch ( level ) {
                case 0:
                    if ( backlight() > 1 )
                        setBacklight( 1 ); // lowest non-off
                    return true;
                    break;
                case 1:
                    setBacklight( 0 ); // off
                    return true;
                    break;
                case 2:
		default:
                    // We're going to suspend the whole machine
                    if ( PowerStatusManager::readStatus().acStatus() != PowerStatus::Online ) {
                        QWSServer::sendKeyEvent( 0xffff, Qt::Key_F34, FALSE, TRUE, FALSE );
                        return true;
                    }
                    break;
            }
            return false;
        }

        void setIntervals( int i1 = 30, int i2 = 20, int i3 = 60 )
        {
            int v [4];

            v [ 0 ] = QMAX( 1000 * i1, 100 );
            v [ 1 ] = QMAX( 1000 * i2, 100 );
            v [ 2 ] = QMAX( 1000 * i3, 100 );
            v [ 3 ] = 0;

            if ( !i1 && !i2 && !i3 )
                QWSServer::setScreenSaverInterval ( 0 );
            else
                QWSServer::setScreenSaverIntervals ( v );
        }

        int backlight ( )
        {
            if ( m_backlight_bright == -1 )
                m_backlight_bright = 255;

            return m_backlight_bright;
        }

        void setBacklight ( int bright )
        {
            if ( bright == -3 ) {
                // Forced on
                m_backlight_forcedoff = false;
                bright = -1;
            }
            if ( m_backlight_forcedoff && bright != -2 )
                return;

            if ( bright == -2 ) {
                // Toggle between off and on
                bright = m_backlight_bright ? 0 : -1;
                m_backlight_forcedoff = !bright;
            }

            m_backlight_bright = bright;

            bright = backlight ( );
            ODevice::inst ( ) -> setDisplayBrightness ( bright );

            m_backlight_bright = bright;
        }

    private:
        bool m_lcd_status;
        int m_backlight_bright;
        bool m_backlight_forcedoff;
};


int login_main ( int argc, char **argv, pid_t ppid )
{
    QWSServer::setDesktopBackground( QImage() );
    LoginApplication *app = new LoginApplication ( argc, argv, ppid );

    app-> setFont ( QFont ( "Helvetica", 10 ));
    app-> setStyle ( new QPEStyle ( ));

    if ( QWSServer::mouseHandler() &&
         QWSServer::mouseHandler()-> inherits("QCalibratedMouseHandler") )
    {
        if ( !QFile::exists ( "/etc/pointercal" )) {
            // Make sure calibration widget starts on top.
            Calibrate *cal = new Calibrate;
            cal-> exec ( );
            delete cal;
        }
    }

    LoginScreenSaver *saver = new LoginScreenSaver;

    saver-> setIntervals ( );
    QWSServer::setScreenSaver ( saver );
    saver-> restore ( );

    LoginWindowImpl *lw = new LoginWindowImpl ( );
    app-> setMainWidget ( lw );
    lw-> setGeometry ( 0, 0, app-> desktop ( )-> width ( ),
                       app-> desktop ( )-> height ( ));
    lw-> show ( );

    int rc = app-> exec ( );

    if ( app-> loginAs ( )) {
        if ( app-> changeIdentity ( )) {
            app-> login ( );
            // if login succeeds, it never comes back
            QMessageBox::critical ( 0, LoginWindowImpl::tr( "Failure" ),
                                    LoginWindowImpl::tr( "Could not start Opie." ));
            rc = 1;
        }
        else {
            QMessageBox::critical ( 0, LoginWindowImpl::tr( "Failure" ),
                                    LoginWindowImpl::tr( "Could not switch to new user identity" ));
            rc = 2;
        }
    }
    return rc;
}

