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

#ifndef THREADUTIL_H
#define THREADUTIL_H

#include <qvaluelist.h>
#include <qobject.h>
#include <qguardedptr.h>

class QSocketNotifier;

namespace ThreadUtil
{

    class Mutex
    {
        friend class WaitCondition;
    public:
        Mutex();
        ~Mutex();

        void lock();
        void unlock();
        bool tryLock();
        bool isLocked();

    private:
        struct Data;
        Data *d;

        Mutex( const Mutex & );
        Mutex &operator=( const Mutex & );
    };

    class AutoLock
    {
    public:
        AutoLock( Mutex &mutex ) : m_mutex( mutex ) { m_mutex.lock(); }
        ~AutoLock() { m_mutex.unlock(); }

        Mutex *operator &() const { return &m_mutex; }

    private:
        Mutex &m_mutex;
    };

    class WaitCondition
    {
    public:
        WaitCondition();
        ~WaitCondition();

        bool wait();
        bool wait( Mutex &mutex );

        void wakeOne();
        void wakeAll();

    private:
        struct Data;
        Data *d;

        WaitCondition( const WaitCondition & );
        WaitCondition &operator=( const WaitCondition & );
    };

    class Thread
    {
    public:
        struct Data;
        friend struct Data;

        Thread();
        virtual ~Thread();

        void start();
        void terminate();

        bool wait();

        bool isRunning() const;

        static void exit();
    protected:
        virtual void run() = 0;

    private:
        Data *d;
    };

    class OnewayNotifier : public QObject
    {
        Q_OBJECT
    public:
        OnewayNotifier();
        ~OnewayNotifier();

        void notify();

    signals:
        void awake();

    private slots:
        void wakeUp();

    private:
        int m_readFd;
        int m_writeFd;
        QSocketNotifier *m_notifier;
    };


    class Channel;

    class ChannelMessage
    {
        friend class Channel;
    public:
        ChannelMessage( int type = -1 );
        virtual ~ChannelMessage();

        int type() const { return m_type; }

        void reply();

    private:
        ChannelMessage( const ChannelMessage & );
        ChannelMessage &operator=( const ChannelMessage );

        int m_type;
        bool m_isCall : 1;
        bool m_replied : 1;
        bool m_inEventHandler : 1;
        Mutex m_guard;
        WaitCondition m_condition;
        QGuardedPtr<Channel> m_channel;
    };

    class Channel : public QObject
    {
        Q_OBJECT
    public:
        enum SendType { OneWay, WaitForReply };
        Channel( QObject *parent = 0, const char *name = 0 );
        virtual ~Channel();

        void send( ChannelMessage *message, SendType type );

    protected:
        virtual void receiveMessage( ChannelMessage *message, SendType type ) = 0;

    private slots:
        void deliver();

    private:
        OnewayNotifier m_notifier;

        struct MsgEnvelope
        {
            MsgEnvelope() : type( OneWay ), msg( 0 ) {}
            MsgEnvelope( SendType _type , ChannelMessage *_msg )
                : type( _type ), msg( _msg ) {}

            SendType type;
            ChannelMessage *msg;
        };

        void deliverOne( const MsgEnvelope &envelope );

        typedef QValueList<MsgEnvelope> MsgEnvelopeList;

        MsgEnvelopeList m_pendingMessages;
        Mutex m_pendingMessagesGuard;

        struct Private;
        Private *d;
    };

}

#endif // THREADUTIL_H
/* vim: et sw=4 ts=4
 */
