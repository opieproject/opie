#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <qfile.h>

#include <qpe/qpeapplication.h>

#include "mainwindow.h"

//#define FSCKED_DISTRIBUTION 1
#ifdef FSCKED_DISTRIBUTION
/*
 * The Zaurus rom
 */
class FixIt {
public:
    FixIt();
    ~FixIt();
    void fixIt();
    /* no real interested in implementing it */
    void breakIt() {

    };
    char* m_file;
};

FixIt::FixIt() {
    /* the new inittab */
    m_file = "#\n# /etc/inittab
#

#   0 - halt (Do NOT set initdefault to this)
#   1 - Single user mode
#   2 - Multiuser, without NFS (The same as 3, if you do not have networking)
#   3 - Full multiuser mode
#   4 - JavaVM(Intent) developer mode
#   5 - JavaVM(Intent)
#   6 - reboot (Do NOT set initdefault to this)
#
id:5:initdefault:

# Specify things to do when starting
si::sysinit:/etc/rc.d/rc.sysinit

l0:0:wait:/root/etc/rc.d/rc 0
l1:1:wait:/etc/rc.d/rc 1
l2:2:wait:/etc/rc.d/rc 2
l3:3:wait:/etc/rc.d/rc 3
l4:4:wait:/etc/rc.d/rc 4
l5:5:wait:/etc/rc.d/rc 5
l6:6:wait:/root/etc/rc.d/rc 6

# Specify things to do before rebooting
um::ctrlaltdel:/bin/umount -a -r > /dev/null 2>&1
sw::ctrlaltdel:/sbin/swapoff -a > /dev/null 2>&1

# Specify program to run on ttyS0
s0:24:respawn:/sbin/getty 9600 ttyS0
#pd:5:respawn:/etc/sync/serialctl

# Specify program to run on tty1
1:2:respawn:/sbin/getty 9600 tty1
ln:345:respawn:survive -l 6 /sbin/launch
#qt:5:respawn:/sbin/qt

# collie sp.
sy::respawn:/sbin/shsync\n";


}
FixIt::~FixIt() {
}
/*
 * the retail Zaurus is broken in many ways
 * one is that pppd is listening on our port...
 * we've to stop it from that and then do kill(SIGHUP,1);
 */
void FixIt::fixIt() {
    ::rename("/etc/inittab", QPEApplication::qpeDir() + "/etc/inittab" );
    QFile file( "/etc/inittab" );
    if ( file.open(IO_WriteOnly | IO_Raw ) ) {
        file.writeBlock(m_file,strlen(m_file) );
    }
    file.close();
    ::kill( SIGHUP, 1 );
}
#endif


int main(int argc, char **argv) {
    QPEApplication app( argc, argv );

#ifdef   FSCKED_DISTRIBUTION
    qWarning("fscked");
    FixIt it;
    it.fixIt();
#endif

    MainWindow mw;
    mw.setCaption(QObject::tr("Opie console") );
    app.showMainWidget( &mw );

    int ap = app.exec();

#ifdef FSCKED_DISTRIBUTION
    /* should add a signal  handler too */
    it.breakIt();
#endif
    return ap;
}
