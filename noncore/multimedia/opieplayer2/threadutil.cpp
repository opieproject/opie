/* This file is part of the KDE project
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "threadutil.h"

#include <qsocketnotifier.h>

#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>

using namespace ThreadUtil;

struct Mutex::Data
{
    Data()
    {
        pthread_mutex_init( &mutex, 0 );
    }
    ~Data()
    {
        pthread_mutex_destroy( &mutex );
    }

    pthread_mutex_t mutex;
};

Mutex::Mutex()
    : d( new Data )
{
}

Mutex::~Mutex()
{
    delete d;
}

void Mutex::lock()
{
    pthread_mutex_lock( &d->mutex );
}

void Mutex::unlock()
{
    pthread_mutex_unlock( &d->mutex );
}

bool Mutex::tryLock()
{
    return pthread_mutex_trylock( &d->mutex ) == 0;
}

bool Mutex::isLocked()
{
    if ( !tryLock() )
        return true;

    unlock();
    return false;
}

struct WaitCondition::Data
{
    Data()
    {
        int result = pthread_cond_init( &waitCondition, 0 );
        assert( result == 0 );
    }
    ~Data()
    {
        pthread_cond_destroy( &waitCondition );
    }

    pthread_cond_t waitCondition;
};

WaitCondition::WaitCondition()
    : d( new Data )
{
}

WaitCondition::~WaitCondition()
{
    delete d;
}

bool WaitCondition::wait()
{
    Mutex m;
    m.lock();
    return wait( m );
}

bool WaitCondition::wait( Mutex &mutex )
{
    return pthread_cond_wait( &d->waitCondition, &mutex.d->mutex );
}

void WaitCondition::wakeOne()
{
    pthread_cond_signal( &d->waitCondition );
}

void WaitCondition::wakeAll()
{
    pthread_cond_broadcast( &d->waitCondition );
}

struct Thread::Data
{
    Data() : isRunning( false ) 
    {}

    pthread_t self;
    Mutex guard;
    bool isRunning;

    WaitCondition finishCondition;
};

extern "C"
{

void _threadutil_terminate_thread( void *arg )
{
    Thread *thr = ( Thread* )arg;

    assert( thr );

    AutoLock locker( thr->d->guard );
    thr->d->isRunning = false;
    thr->d->finishCondition.wakeAll();
}

void *_threadutil_start_thread( void *arg )
{
    Thread *thr = ( Thread* )arg;

    pthread_cleanup_push( _threadutil_terminate_thread, thr );

    thr->d->isRunning = true;
    thr->run();

    pthread_cleanup_pop( true );

    Thread::exit();
    return 0; // never reached
}

}

Thread::Thread()
    : d( new Data )
{
}

Thread::~Thread()
{
    assert( d->isRunning == false );
    delete d;
}

void Thread::start()
{
    AutoLock lock( d->guard );

    if ( d->isRunning ) {
        qDebug( "ThreadUtil::Thread::start() called for running thread." );
        return;
    }

    pthread_attr_t attributes;
    pthread_attr_init( &attributes );
    pthread_attr_setscope( &attributes, PTHREAD_SCOPE_SYSTEM );
    int err = pthread_create( &d->self, &attributes, _threadutil_start_thread, ( void* )this );
    if ( err != 0 ) {
        qDebug( "ThreadUtil::Thread::start() : can't create thread: %s", strerror( err ) );
        pthread_attr_destroy( &attributes );
        return;
    }
    pthread_attr_destroy( &attributes );
}

void Thread::terminate()
{
    AutoLock lock( d->guard );
    if ( !d->isRunning )
        return;

    pthread_cancel( d->self );
}

bool Thread::wait()
{
    AutoLock lock( d->guard );
    if ( !d->isRunning )
        return true;

    return d->finishCondition.wait( d->guard );
}

void Thread::exit()
{
    pthread_exit( 0 );
}

OnewayNotifier::OnewayNotifier()
{
    int fds[ 2 ];
    pipe( fds );
    m_readFd = fds[ 0 ];
    m_writeFd = fds[ 1 ];

    m_notifier = new QSocketNotifier( m_readFd, QSocketNotifier::Read );
    connect( m_notifier, SIGNAL( activated( int ) ),
             this, SLOT( wakeUp() ) );
}

OnewayNotifier::~OnewayNotifier()
{
    delete m_notifier;

    ::close( m_readFd );
    ::close( m_writeFd );
}

void OnewayNotifier::notify()
{
    const char c = 42;
    ::write( m_writeFd, &c, 1 );
}

void OnewayNotifier::wakeUp()
{
    char c = 0;

    if ( ::read( m_readFd, &c, 1 ) != 1 )
        return;

    emit awake();
}

ChannelMessage::ChannelMessage( int type )
    : m_type( type ), m_isCall( false ), m_replied( false ),
      m_inEventHandler( false )
{
}

ChannelMessage::~ChannelMessage()
{
    if ( m_guard.isLocked() )
        m_guard.unlock();
}

void ChannelMessage::reply()
{
    if ( !m_isCall )
    {
        qDebug( "ChannelMessage::reply() - can't reply oneway message!" );
        return;
    }

    if ( m_inEventHandler )
    {
        m_replied = true;
        return;
    }

    m_condition.wakeOne();
    m_guard.unlock();
}

struct Channel::Private
{
    Private()
    {
        ownerThread = pthread_self();
    }

    pthread_t ownerThread;
};

Channel::Channel( QObject *parent, const char *name )
    : QObject( parent, name ), d( new Private )
{
    connect( &m_notifier, SIGNAL( awake() ),
             this, SLOT( deliver() ) );
}

Channel::~Channel()
{
    delete d;
}

void Channel::send( ChannelMessage *message, SendType type )
{
    if ( type == WaitForReply )
    {
        message->m_guard.lock();
        message->m_isCall = true;
    }

    m_pendingMessagesGuard.lock();
    m_pendingMessages << MsgEnvelope( type, message );
    m_pendingMessagesGuard.unlock();

    if ( d->ownerThread == pthread_self() ) {
        assert( type != WaitForReply );

        deliver();
    }
    else
        m_notifier.notify();
    //QThread::postEvent( this, new QCustomEvent( QEvent::User, envelope ) );

    if ( type == WaitForReply )
    {
        message->m_condition.wait( message->m_guard );
        message->m_guard.unlock();
    }
}

void Channel::deliver()
{
    AutoLock lock( m_pendingMessagesGuard );

    while ( !m_pendingMessages.isEmpty() ) {
        MsgEnvelope envelope = m_pendingMessages.first();

        m_pendingMessages.remove( m_pendingMessages.begin() );

        m_pendingMessagesGuard.unlock();
        deliverOne( envelope );
        m_pendingMessagesGuard.lock();
    }
}

void Channel::deliverOne( const MsgEnvelope &envelope )
{
    ChannelMessage *msg = envelope.msg;

    assert( msg );

    if ( envelope.type == WaitForReply )
    {
        msg->m_guard.lock();
        msg->m_inEventHandler = true;
    }

    receiveMessage( msg, envelope.type );

    if ( envelope.type == WaitForReply )
    {
        msg->m_inEventHandler = false;
        if ( msg->m_replied )
        {
            msg->m_condition.wakeOne();
            // this is a bit tricky. we unlock only when we reply.
            // reply() does an unlock as well.
            msg->m_guard.unlock();
        }
    }
}

/* vim: et sw=4 ts=4
 */
