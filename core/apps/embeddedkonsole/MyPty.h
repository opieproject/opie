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


class MyPty : public QObject
{
Q_OBJECT

  public:

    MyPty();
    ~MyPty();

    /*!
        having a `run' separate from the constructor allows to make
        the necessary connections to the signals and slots of the
        instance before starting the execution of the client.
    */
    int run(const char* pgm, QStrList & args, const char* term, int addutmp);

  public slots:

    void send_bytes(const char* s, int len);
    void setSize(int lines, int columns);
    void error();

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
    void block_in(const char* s, int len);

  public:

    void send_byte(char s);
//    void send_string(const char* s);

    const char* deviceName();

  protected slots:
      void readPty();
      void donePty();
      
  private:
    int  openPty();

  private:

    char ptynam[16]; // "/dev/ptyxx" | "/dev/ptmx"
    char ttynam[16]; // "/dev/ttyxx" | "/dev/pts/########..."
    int fd;
    int cpid;
};

#endif
