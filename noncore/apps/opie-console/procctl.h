#ifndef OPIE_PROC_CTL_H
#define OPIE_PROC_CTL_H

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <qmap.h>


struct ProcContainer {
    pid_t pid;
    int fd;
    int status;
    ProcContainer* prev;
};

class ProcCtl {
private:
    ProcCtl();
public:
    ~ProcCtl();

    static ProcCtl* self();
    int status(pid_t)const;
    void add( pid_t, int fd );
    void remove( pid_t );
    void remove( ProcContainer );
private:
    static void signal_handler(int);
    static ProcContainer* m_last;
    static ProcCtl* m_self;

};

#endif
