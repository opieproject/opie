#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include <qsocketnotifier.h>

#include "io_layer.h"
#include "procctl.h"
#include "filereceive.h"

FileReceive::FileReceive( Type t, IOLayer* lay, const QString& dir )
    : ReceiveLayer(lay, dir ), m_type( t )
{
    m_fd = -1;
    m_not = 0l;
    m_proc = 0l;
}
FileReceive::~FileReceive() {
}
void FileReceive::receive() {
    receive( currentDir() );
}
void FileReceive::receive( const QString& dir ) {
    m_prog = -1;
    m_fd = layer()->rawIO();
    m_curDir = dir;

    if (pipe( m_comm ) < 0 )
        m_comm[0] = m_comm[1] = 0;
    if (pipe( m_info ) < 0 )
        m_info[0] = m_info[1] = 0;

    m_pid = fork();
    switch( m_pid ) {
    case -1:
        //emit error
        slotExec();
        break;
        /* child */
    case 0: {
        setupChild();
        char* typus =  NULL;
        switch(m_type ) {
        case SZ:
            break;
        case SX:
            typus = "-X";
            break;
        case SY:
            typus = "--ymodem";
            break;
        }

        /* we should never return from here */
        if( m_type == SX )
            // FIXME: file name should be configurable - currently we ensure it
            // doesn't get overwritten by -E (--rename)
            execlp("rz", "rz",  typus, "--overwrite", QObject::tr("SynchronizedFile").latin1(), NULL );
        else
            execlp("rz", "rz",  typus, "--overwrite", NULL );

        char resultByte = 1;
        if (m_info[1] )
            ::write(m_info[1], &resultByte, 1 );

        _exit( -1 );
        break;
    }
    default: {
        if ( m_info[1] )
            close( m_info[1] );

        if ( m_info[0] ) for (;;) {
            char resultByte; int len;
            len = read(m_info[0], &resultByte, 1 );
            /* len == 1 start up failed */
            if ( len == 1 ) {
                emit error( StartError, tr("Could not start") );
                return;
            }
            if ( len == -1 )
                if ( (errno == ECHILD ) || (errno == EINTR ) )
                    continue;

            // len == 0 or something like this
            break;
        }

        if ( m_info[0] )
            close( m_info[0] );

        m_not = new QSocketNotifier(m_comm[0],  QSocketNotifier::Read );
        connect(m_not, SIGNAL(activated(int) ),
                this, SLOT(slotRead() ) );
        if ( pipe(m_term) < 0 )
            m_term[0] = m_term[1] = 0;

        ProcCtl::self()->add(m_pid, m_term[1] );
        m_proc = new QSocketNotifier(m_term[0], QSocketNotifier::Read );
        connect(m_proc, SIGNAL(activated(int) ),
                this, SLOT(slotExec() ) );

    }
        break;

    }

}
void FileReceive::cancel() {
    ::kill(m_pid, 9 );
}
void FileReceive::setupChild() {
    changeDir( currentDir() );
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
void FileReceive::slotRead() {
    QByteArray ar(4096);
    int len = read(m_comm[0], ar.data(), 4096 );
    for (int i = 0; i < len; i++ ) {
        // printf("%c", ar[i] );
    }
    ar.resize( len );
    QString str( ar );
}
void FileReceive::slotExec() {
    char buf[2];
    ::read(m_term[0], buf, 1 );
    delete m_proc;
    delete m_not;
    m_not = m_proc = 0l;
    close( m_term[0] );
    close( m_term[1] );
    close( m_comm[0] );
    close( m_comm[1] );
    layer()->closeRawIO(m_fd);
    emit received(QString::null);

}
