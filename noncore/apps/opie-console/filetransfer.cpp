#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <qcstring.h>
#include <qsocketnotifier.h>

#include <opie/oprocess.h>

#include "procctl.h"
#include "filetransfer.h"


FileTransfer::FileTransfer( Type t, IOLayer* lay )
    : FileTransferLayer( lay ), m_type( t ), m_pid ( 0 ) {
    signal(SIGPIPE,  SIG_IGN );

    m_pid = 0;
    m_not = 0l;
    m_proc = 0l;
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
    m_prog =-1;
    m_fd = layer()->rawIO();
//
//    m_fd = ::open("/dev/ttyS0", O_RDWR);

    m_file = file;
    if ( pipe( m_comm ) < 0 )
        m_comm[0] = m_comm[1] = 0;
    if ( pipe( m_info ) < 0 )
        m_info[0] = m_info[1] = 0;


    m_pid = fork();
    switch( m_pid ) {
    case -1:
        emit error( StartError, tr("Was not able to fork") );
        slotExec();
        break;
    case 0:{
        setupChild();
        /* exec */
        char* verbose = "-vv";
        char* binray = "-b";


        char* typus;
        switch(m_type ) {
        case SZ:
            typus = "";
            break;
        case SX:
            typus = "-X";
            break;
        case SY:
            typus = "--ymodem";
            break;
        }

        /* we should never return from here */
        execlp("sz", "sz", verbose,  binray, file.latin1(), typus, NULL );

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



        /* replace by QSocketNotifier!!! */
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
/*
 * let's call the one with the filename
 */
void FileTransfer::sendFile( const QFile& file ) {
    sendFile( file.name() );
}

/*
 * setting up communication
 * between parent child and ioLayer
 */
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

/*
 * read from the stderr of the child
 * process
 */
void FileTransfer::slotRead() {
    QByteArray ar(4096);
    int len = read(m_comm[0], ar.data(), 4096 );
    for (int i = 0; i < len; i++ ) {
        // printf("%c", ar[i] );
    }
    ar.resize( len );
    QString str( ar );
    QStringList lis = QStringList::split(' ', str );
    /*
     * Transfer finished.. either complete or incomplete
     */
    if ( lis[0].simplifyWhiteSpace() == "Transfer" ) {
        return;
    }
    /*
     * do progress reading
     */
    slotProgress( lis );


}
/*
 * find the progress
 */
void FileTransfer::slotProgress( const QStringList& list ) {
    if ( m_type != SZ )
        return;
    bool complete = true;
    int min, sec;
    int bps;
    unsigned long sent, total;

    min = sec = bps = -1;
    sent = total = 0;

    // Data looks like this
    //  0      1         2          3      4     5
    // Bytes Sent 65536/11534336 BPS:7784 ETA 24:33
    QStringList progi = QStringList::split('/', list[2].simplifyWhiteSpace()  );
    sent  = progi[0].toULong(&complete );
    if (!complete ) return;

    total = progi[1].toULong(&complete );
    if (!complete || total == 0) {
        return;
    }


    double pro = (double)sent/total;
    int prog = pro * 100;

    // speed
    progi = QStringList::split(':', list[3].simplifyWhiteSpace() );
    bps = progi[1].toInt();

    // time
    progi = QStringList::split(':', list[5].simplifyWhiteSpace() );
    min = progi[0].toInt();
    sec = progi[1].toInt();


    if ( prog > m_prog ) {
        m_prog = prog;
        emit progress(m_file, m_prog, bps, -1, min , sec );
    }

}
void FileTransfer::cancel() {
    if(m_pid > 0) ::kill(m_pid,9 );
    
}
void FileTransfer::slotExec() {
    char buf[2];
    ::read(m_term[0], buf, 1 );
    delete m_proc;
    delete m_not;
    m_proc = m_not = 0l;
    close( m_term[0] );
    close( m_term[1] );
    close( m_comm[0] );
    close( m_comm[1] );
    layer()->closeRawIO( m_fd );
    emit sent();
    m_pid = 0;
}
