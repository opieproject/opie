#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <qcstring.h>
#include <qsocketnotifier.h>

#include <opie/oprocess.h>

#include "filetransfer.h"

bool FileTransfer::terminate = false;
pid_t FileTransfer::m_pid;

FileTransfer::FileTransfer( Type t, IOLayer* lay )
    : FileTransferLayer( lay ), m_type( t ) {
    signal( SIGCHLD, signal_handler );
}
FileTransfer::~FileTransfer() {
}

/**
 * now we will send the file.
 *
 * we request an fd. The IOLayer should be closed
 * then we will setup a pipe for progress communication
 * then we will dup2 the m_fd in the forked process
 * to do direct IO from and to the fd
 */
void FileTransfer::sendFile( const QString& file ) {
    m_fd = layer()->rawIO();
//
//    m_fd = ::open("/dev/ttyS0", O_RDWR);


    if ( pipe( m_comm ) < 0 )
        m_comm[0] = m_comm[1] = 0;
    if ( pipe( m_info ) < 0 )
        m_info[0] = m_info[1] = 0;

    qWarning("output:"+file );
    m_pid = fork();
    switch( m_pid ) {
    case 0:{
        setupChild();
        /* exec */
        char* verbose = "-vv";
        char* binray = "-b";


        /* we should never return from here */
        execlp("sz", "sz", verbose, binray, file.latin1(), NULL );

        /* communication for error!*/
        char resultByte =1;
        if (m_info[1] )
            write(m_info[1], &resultByte, 1 );
        _exit( -1 );
        break;
    }
    default:{
        if ( m_info[1] )
            close( m_info[1] );
        if ( m_info[0] ) for (;;) {
            char resultByte; int len;
            len = read(m_info[0], &resultByte, 1 );
            /* len == 1 start up failed */
            if ( len == 1 )
                return;
            if ( len == -1 )
                if ( (errno == ECHILD ) || (errno == EINTR ) )
                    continue;

            // len == 0 or something like this
            break;
        }
        if ( m_info[0] )
            close( m_info[0] );

        terminate = false;
        fd_set fds;
        struct timeval timeout;
        int sel;

        /* replace by QSocketNotifier!!! */
        while (!terminate) {
            FD_ZERO( &fds );
            FD_SET( m_comm[0], &fds );
            sel = select( m_comm[0]+1, &fds, NULL, NULL, &timeout );
            if (sel ) {
                if ( FD_ISSET(m_comm[0], &fds ) ) {
                    printf("out:");
                    QByteArray ar(4096);
                    int len = read(m_comm[0], ar.data(), 4096 );
                    for (int i = 0; i < len; i++ ) {
                        // printf("%c", ar[i] );
                    }
                    printf("\n");
                }
            }
        }
        break;
    }
    }
}
void FileTransfer::sendFile( const QFile& file ) {
    sendFile( file.name() );
}
void FileTransfer::signal_handler(int ) {
    int status;
    signal( SIGCHLD, signal_handler );
    waitpid( m_pid, &status, WNOHANG );
    terminate = true;
}
void FileTransfer::setupChild() {
    /*
     * we do not want to read from our
     * information channel
     */
    if (m_info[0] )
        close(m_info[0] );
    /*
     * FD_CLOEXEC will close the
     * fd on successfull exec
     */
    if (m_info[1] )
        fcntl(m_info[1], F_SETFD, FD_CLOEXEC );

    if (m_comm[0] )
        close( m_comm[0] );
    /*
     * now set the communication
     * m_fd STDIN_FILENO
     *      STDOUT_FILENO
     *      STDERR_FILENO
     */
    dup2( m_fd, STDIN_FILENO );
    dup2( m_fd, STDOUT_FILENO );
    dup2( m_comm[1], STDERR_FILENO );
}
