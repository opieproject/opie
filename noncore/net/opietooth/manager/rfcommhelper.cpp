#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>


#include "rfcommhelper.h"

using namespace OpieTooth;

bool RfCommHelper::terminate = false;
pid_t RfCommHelper::m_pid;

RfCommHelper::RfCommHelper()
    : m_connected( false )
{
    signal( SIGCHLD, signal_handler );
}
RfCommHelper::~RfCommHelper() {
    detach();
}
QCString RfCommHelper::attachedDevice() const {
    return m_device;
}
void RfCommHelper::detach() {
    if (m_connected )
        ::kill( m_pid,  9 );
    if ( m_in2out[0] )
        close(m_in2out[0] );
    if ( m_in2out[1] )
        close(m_in2out[1] );
    if ( m_out2in[0] )
        close(m_out2in[0] );
    if ( m_out2in[1] )
        close(m_out2in[1] );
}
bool RfCommHelper::attach( const QString& bd_addr, int port ) {
    int i =0;
    bool ok = false;
    while (!ok   ) {
        if (i == 4) break;
        ok = connect( i,  bd_addr,  port );
        i++;
    }
    return ok;
}
/*
 * not implemented yet
 */
void RfCommHelper::regroup() {

}
bool RfCommHelper::connect(int devi, const QString& bdaddr, int port) {
    m_connected = false;
    if ( pipe(m_fd) < 0 )
        m_fd[0] = m_fd[1] = 0;
    if ( pipe(m_in2out)  < 0 )
        m_in2out[0] = m_in2out[1] = 0;
    if ( pipe(m_out2in ) < 0 )
        m_out2in[0] = m_out2in[1] = 0;

    m_pid = fork();
    switch( m_pid ) {
    case -1:
        return false;
        break;
    /*
     * This is the child code.
     * We do some fd work
     * and then we'll execlp
     * to start it up
     */
    case 0:{ // child code
        setupComChild();
        char por[15];
        char dev[15];
        sprintf( por, "%d", port );
        sprintf( dev, "%d", devi );
        execlp( "rfcomm", "rfcomm",  dev, bdaddr.latin1(), por, NULL );
        char resultByte = 1;
        if ( m_fd[1] )
            write(m_fd[1], &resultByte, 1 );
        _exit( -1 );
        break;
    }
    /*
     * The Parent. We'll first wait for fd[0] to fill
     * up.
     * Then we will wait for out2in[0] to fill up and then
     * we will parse it.
     * maybe the signal handler gets it's turn in this case we return
     * false
     * otheriwse we will parse the Output and either return true
     * or false
     */
    default: {
        if ( m_fd[1] )
            close( m_fd[1] );
        if ( m_fd[0] ) for (;;) {
            char resultByte;
            int len;
            len = read(m_fd[0], &resultByte, 1 );
            if ( len == 1 ) { // it failed to execute
                return false;
            }
            if ( len == -1 )
                if ( (errno == ECHILD ) || (errno == EINTR ) )
                    continue; // the other process is not yet ready?

            break;
        }
        if ( m_fd[0] )
            close( m_fd[0] );
        terminate = false;
        fd_set fds;
        struct timeval timeout;
        int sel;
        while (!terminate ) {
            FD_ZERO( &fds );
            FD_SET( m_in2out[0], &fds );
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;

            sel = select( m_in2out[0]+1, &fds, NULL, NULL,  &timeout );
            if ( sel )
                if (FD_ISSET(m_in2out[0], &fds ) ) {
                    char buf[2048];
                    int len;
                    buf[0]  = 0;
                    len = read( m_in2out[0], buf,  sizeof(buf) );
                    if ( len > 0 ) {
                        QCString string( buf );
                        if (string.left(9) == "Connected" ) {
                            m_connected = true;
                            m_device = m_device.sprintf("/dev/tty%d", devi );
                            break; // we got connected
                        };
                    }
                    // now parese it
                }else {// time out
                    // 5 seconds without input check terminate?
                    //
                    ;
                }
        }
        break;
    }
    }
    return !terminate;
}


void RfCommHelper::setupComChild() {
    if ( m_fd[0] )
        close(m_fd[0]);
    if ( m_fd[1] )
        fcntl( m_fd[1] ,  F_SETFD,  FD_CLOEXEC);

    /* duplicating pipes and making them STDIN and STDOUT
     * of the new process
     * [0] is for reading
     * [1] is for writing
     */
    dup2( m_out2in[0], STDIN_FILENO );
    dup2( m_in2out[1], STDOUT_FILENO );


};
void RfCommHelper::signal_handler(int) {
    int status;
    terminate = true;
    signal( SIGCHLD, signal_handler );
    waitpid( m_pid, &status, WNOHANG );
}
