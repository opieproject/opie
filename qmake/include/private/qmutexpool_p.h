#ifndef QMUTEXPOOL_H
#define QMUTEXPOOL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QSettings. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifdef QT_THREAD_SUPPORT

#include "qmutex.h"
#include "qmemarray.h"

class QMutexPool
{
public:
    QMutexPool( bool recursive = FALSE, int size = 17 );
    ~QMutexPool();

    QMutex *get( void *address );

private:
    QMutex mutex;
    QMemArray<QMutex*> mutexes;
    bool recurs;
};

extern QMutexPool *qt_global_mutexpool;

#endif // QT_THREAD_SUPPORT

#endif // QMUTEXPOOL_H
