/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [MyPty.h]                 Pseudo Terminal Device                           */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole - an X terminal for KDE                       */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*									      */
/* Ported Konsole to Qt/Embedded                                              */
/*									      */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*									      */
/* -------------------------------------------------------------------------- */

/*! \file
*/

#ifndef MY_PTY_H
#define MY_PTY_H

#include <qobject.h>
#include <qstrlist.h>

#include "io_layer.h"

class Profile;
class QSocketNotifier;
class MyPty : public IOLayer
{
    Q_OBJECT
public:

    MyPty(const Profile&);
    ~MyPty();



    QString identifier()const;
    QString name()const;
    QBitArray supports()const;

  public slots:
 /*!
      having a `run' separate from the constructor allows to make
      the necessary connections to the signals and slots of the
      instance before starting the execution of the client.
    */
    void start();
    int run(const char* pgm, QStrList & args , const char* term, int addutmp);
    bool open();
    void close();
    void reload( const Profile& );
    void setSize(int lines, int columns);
    void error();
    bool isConnected() { return true; };
  signals:

    /*!
        emitted when the client program terminates.
        \param status the wait(2) status code of the terminated client program.
    */
    void done(int status);

    /*!
        emitted when a new block of data comes in.
        \param s - the data
        \param len - the length of the block
    */
    void received(const QByteArray&);

  public slots:

    void send(const QByteArray& );

private:
    const char* deviceName();

protected slots:
      void readPty();
      void donePty();

private:
    int  openPty();

private:

    char m_ptynam[16]; // "/dev/ptyxx" | "/dev/ptmx"
    char m_ttynam[16]; // "/dev/ttyxx" | "/dev/pts/########..."
    int m_fd;
    int m_cpid;
    QSocketNotifier* m_sn_e;
    QSocketNotifier* m_sn_r;
};

#endif
