/* $Id: forwarder.cc,v 1.1 2006-04-14 19:22:37 korovkin Exp $
 *	Bluetooth serial forwarder class implementation
 *
 *	(c) Copyright 2006 GPL
 *
 *	This software is provided under the GNU public license, incorporated
 *	herein by reference. The software is provided without warranty or 
 *	support.
 */
#include "bt-serial.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <qapplication.h>
#include <opie2/oprocctrl.h>
#include "forwarder.h"
using namespace OpieTooth;
using namespace Opie::Core;
using namespace Opie::Core::Internal;

SerialForwarder::SerialForwarder(QString& devName, int dspeed) : 
    OProcess(), device(devName), speed(dspeed)
{
    status = false;
}

SerialForwarder::~SerialForwarder()
{
    stop();
}


bool SerialForwarder::start(RunMode runmode, Communication comm)
{
    int htmp; //temporary device
    int result; //call result

    if ( runs ) {
        return false;  // cannot start a process that is already running
        // or if no executable has been assigned
    }
    //First, check if serial device is usable
    htmp = ::openSerial(device);
    if (htmp < 0)
        return false;
    close(htmp);

    run_mode = runmode;
    status = 0;

    if(::bt_serialStart() < 0)
        return false;

    if ( !setupCommunication( comm ) )
        qWarning( "Could not setup Communication!" );

    // We do this in the parent because if we do it in the child process
    // gdb gets confused when the application runs from gdb.
    uid_t uid = getuid();
    gid_t gid = getgid();
#ifdef HAVE_INITGROUPS

    struct passwd *pw = getpwuid( uid );
#endif

    int fd[ 2 ];
    if ( 0 > pipe( fd ) ) {
        fd[ 0 ] = fd[ 1 ] = 0; // Pipe failed.. continue
    }

    runs = true;

    QApplication::flushX();

    // WABA: Note that we use fork() and not vfork() because
    // vfork() has unclear semantics and is not standardized.
    pid_ = fork();

    if ( 0 == pid_ ) {
        if ( fd[ 0 ] )
            close( fd[ 0 ] );
        if ( !runPrivileged() ) {
            setgid( gid );
#if defined( HAVE_INITGROUPS)

            if ( pw )
                initgroups( pw->pw_name, pw->pw_gid );
#endif

            setuid( uid );
        }
        // The child process
        if ( !commSetupDoneC() )
            qWarning( "Could not finish comm setup in child!" );

        setupEnvironment();

        // Matthias
        if ( run_mode == DontCare )
            setpgid( 0, 0 );
        // restore default SIGPIPE handler (Harri)
        struct sigaction act;
        sigemptyset( &( act.sa_mask ) );
        sigaddset( &( act.sa_mask ), SIGPIPE );
        act.sa_handler = SIG_DFL;
        act.sa_flags = 0;
        sigaction( SIGPIPE, &act, 0L );

        // We set the close on exec flag.
        // Closing of fd[1] indicates that the execvp succeeded!
        if ( fd[ 1 ] )
            fcntl( fd[ 1 ], F_SETFD, FD_CLOEXEC );
        do {
            BTSerialConn conn; //Connection handler
            if ( fd[ 1 ] ) {
                ::close(fd[1]);
                fd[1] = 0;
            }
            result = ::bt_serialForward(&conn, device);
        } while(result == 0);

        char resultByte = 1;
        if ( fd[ 1 ] )
            write( fd[ 1 ], &resultByte, 1 );
        _exit( -1 );
    }
    else if ( -1 == pid_ ) {
        // forking failed

        runs = false;
        return false;
    }
    else {
        if ( fd[ 1 ] )
            close( fd[ 1 ] );
        // the parent continues here

        // Discard any data for stdin that might still be there
        input_data = 0;

        // Check whether client could be started.
        if ( fd[ 0 ] ) {
            for ( ;; ) {
                char resultByte;
                int n = ::read( fd[ 0 ], &resultByte, 1 );
                if ( n == 1 ) {
                    // Error
                    runs = false;
                    close( fd[ 0 ] );
                    pid_ = 0;
                    return false;
                }

                if ( n == -1 ) {
                    if ( ( errno == ECHILD ) || ( errno == EINTR ) )
                        continue; // Ignore
                }
                break; // success
            }
        }

        if ( fd[ 0 ] )
            close( fd[ 0 ] );

        if ( !commSetupDoneP() )   // finish communication socket setup for the parent
            qWarning( "Could not finish comm setup in parent!" );

        if ( run_mode == Block ) {
            commClose();

            // The SIGCHLD handler of the process controller will catch
            // the exit and set the status
            while ( runs ) {
                OProcessController::theOProcessController->
                slotDoHousekeeping( 0 );
            }
            runs = FALSE;
            emit processExited( this );
        }
    }
    return true;
}

/*
 * Stop forwarding process
 */
int SerialForwarder::stop()
{
    kill(SIGTERM);
    ::bt_serialStop();
    return 0;
}
//eof
