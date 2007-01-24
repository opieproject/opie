#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include "procctl.h"

ProcContainer *ProcCtl::m_last = 0;
ProcCtl* ProcCtl::m_self = 0;

ProcCtl::ProcCtl() {
    signal( SIGCHLD, signal_handler );
}
ProcCtl::~ProcCtl() {
}
ProcCtl* ProcCtl::self() {
    if (!m_self ) {
        m_self = new ProcCtl;
    }
    return m_self;
}
void ProcCtl::add(pid_t pi,  int fd ) {
    ProcContainer * con = new ProcContainer;
    //memset(con, 0, sizeof(con) );
    con->pid = pi;
    con->fd = fd;
    con->status = 0;
    con->prev = m_last;

    m_last = con;

}
void ProcCtl::remove( pid_t pi ) {
    /*
     * We first check if the last item
     * is equal to  pi the we
     *
     */
    ProcContainer* con;
    if (m_last->pid == pi ) {
        con = m_last;
        m_last = con->prev;
        delete con;
        return;
    }

    con = m_last;
    ProcContainer* forw = 0l;
    while (con ) {
        /* remove it */
        if ( pi == con->pid ) {
            if (forw)
		forw->prev = con->prev;
	    else
		forw = con->prev;
            delete con;
            return;
        }

        forw = con;
        con = con->prev;
    }

}
void ProcCtl::remove( ProcContainer con ) {
    remove( con.pid );
}
int ProcCtl::status(pid_t pid )const{
    ProcContainer *con = m_last;
    while (con) {
        if (con->pid == pid )
            return con->status;
        con = con->prev;
    }
    return -1;
}
void ProcCtl::signal_handler(int) {
    int status;
    signal( SIGCHLD,  signal_handler );
    pid_t pi = waitpid( -1, &status, WNOHANG  );

    /*
     * find the container for pid
     *
     */
    if ( pi < 0 ) {
        return;
    }

    ProcContainer* con = m_last;
    while (con) {
        if ( con->pid == pi ) {
            con->status = status;
            char result = 1;
            /* give a 'signal' */
            ::write(con->fd, &result, 1 );
        }
        con = con->prev;
    }
}
