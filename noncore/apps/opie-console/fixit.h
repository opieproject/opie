#ifndef FIX_IT_H
#define FIX_IT_H

#include <opie2/oprocess.h>
#include <opie2/odebug.h>

#include <qpe/global.h>
#include <qpe/qpeapplication.h>

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


/*
 * The Zaurus rom
 */
class FixIt {
public:
    FixIt();
    void fixIt();
    /* no real interested in implementing it */
    void breakIt();
    char* m_file;
};


#endif

