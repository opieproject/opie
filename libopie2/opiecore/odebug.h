/*
                             This file is part of the Opie Project
                             (C) 2003 Michael 'Mickey' Lauer (mickey@tm.informatik.uni-frankfurt.de)
                             Inspired by the KDE debug classes, which are
                             (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
                             (C) 2002 Holger Freyther (freyther@kde.org)
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef ODEBUG_H
#define ODEBUG_H

#include <qstring.h>

class QWidget;
class QDateTime;
class QDate;
class QTime;
class QPoint;
class QSize;
class QRect;
class QRegion;
class QStringList;
class QColor;
class QBrush;

namespace Opie {
namespace Core {

class odbgstream;
class ondbgstream;

#ifdef __GNUC__
#define o_funcinfo "[" << __PRETTY_FUNCTION__ << "] "
#else
#define o_funcinfo "[" << __FILE__ << ":" << __LINE__ << "] "
#endif

#define o_lineinfo "[" << __FILE__ << ":" << __LINE__ << "] "

#define owarn odWarning()
#define oerr odError()
#define odebug odDebug()
#define ofatal odFatal()
#define oendl "\n"

class odbgstreamprivate;
/**
 * odbgstream is a text stream that allows you to print debug messages.
 * Using the overloaded "<<" operator you can send messages. Usually
 * you do not create the odbgstream yourself, but use @ref odDebug() (odebug)
 * @ref odWarning() (owarn), @ref odError() (oerr) or @ref odFatal (ofatal) to obtain one.
 *
 * Example:
 * <pre>
 *    int i = 5;
 *    odebug << "The value of i is " << i << oendl;
 * </pre>
 * @see odbgstream
 */

/*======================================================================================
 * odbgstream
 *======================================================================================*/

class odbgstream
{
  public:
  /**
   * @internal
   */
    odbgstream(unsigned int _area, unsigned int _level, bool _print = true);
    odbgstream(const char * initialString, unsigned int _area, unsigned int _level, bool _print = true);
    odbgstream(odbgstream &str);
    odbgstream(const odbgstream &str);
    virtual ~odbgstream();

    /**
     * Prints the given value.
     * @param i the boolean to print (as "true" or "false")
     * @return this stream
     */
    odbgstream &operator<<(bool i);
    /**
     * Prints the given value.
     * @param i the short to print
     * @return this stream
     */
    odbgstream &operator<<(short i);
    /**
     * Prints the given value.
     * @param i the unsigned short to print
     * @return this stream
     */
    odbgstream &operator<<(unsigned short i);
    /**
     * Prints the given value.
     * @param i the char to print
     * @return this stream
     */
    odbgstream &operator<<(char i);
    /**
     * Prints the given value.
     * @param i the unsigned char to print
     * @return this stream
     */
    odbgstream &operator<<(unsigned char i);
    /**
     * Prints the given value.
     * @param i the int to print
     * @return this stream
     */
    odbgstream &operator<<(int i);
    /**
     * Prints the given value.
     * @param i the unsigned int to print
     * @return this stream
     */
    odbgstream &operator<<(unsigned int i);
    /**
     * Prints the given value.
     * @param i the long to print
     * @return this stream
     */
    odbgstream &operator<<(long i);
    /**
     * Prints the given value.
     * @param i the unsigned long to print
     * @return this stream
     */
    odbgstream &operator<<(unsigned long i);
    /**
     * Flushes the output.
     */
    virtual void flush();
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    odbgstream &operator<<(const QString& string);
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    odbgstream &operator<<(const char *string);
    /**
     * Prints the given value.
     * @param string the string to print
     * @return this stream
     */
    odbgstream &operator<<(const QCString& string);
    /**
     * Prints the given value.
     * @param p a pointer to print (in number form)
     * @return this stream
     */
    odbgstream& operator<<(const void * p);
    /**
     * Prints the given value.
     * @param d the double to print
     * @return this stream
     */
    odbgstream& operator<<(double d);
    /**
     * Prints the string @p format which can contain
     * printf-style formatted values.
     * @param format the printf-style format
     * @return this stream
     */
    odbgstream &form(const char *format, ...);
    /** Operator to print out basic information about a QWidget.
     *  Output of class names only works if the class is moc'ified.
     * @param widget the widget to print
     * @return this stream
     */
    odbgstream& operator<< (QWidget* widget);

    /**
     * Prints the given value.
     * @param dateTime the datetime to print
     * @return this stream
     */
    odbgstream& operator<< ( const QDateTime& dateTime );

    /**
     * Prints the given value.
     * @param date the date to print
     * @return this stream
     */
    odbgstream& operator<< ( const QDate& date );

    /**
     * Prints the given value.
     * @param time the time to print
     * @return this stream
     */
    odbgstream& operator<< ( const QTime& time );

    /**
     * Prints the given value.
     * @param point the point to print
     * @return this stream
     */
    odbgstream& operator<< ( const QPoint& point );

    /**
     * Prints the given value.
     * @param size the QSize to print
     * @return this stream
     */
    odbgstream& operator<< ( const QSize& size );

    /**
     * Prints the given value.
     * @param rect the QRect to print
     * @return this stream
     */
    odbgstream& operator<< ( const QRect& rect);

    /**
     * Prints the given value.
     * @param region the QRegion to print
     * @return this stream
     */
    odbgstream& operator<< ( const QRegion& region);

    /**
     * Prints the given value.
     * @param list the stringlist to print
     * @return this stream
     */
    odbgstream& operator<< ( const QStringList& list);

    /**
     * Prints the given value.
     * @param color the color to print
     * @return this stream
     */
    odbgstream& operator<< ( const QColor& color);

    /**
     * Prints the given value.
     * @param brush the brush to print
     * @return this stream
     */
    odbgstream& operator<< ( const QBrush& brush );

 private:
    QString output;
    unsigned int area, level;
    bool print;
    odbgstreamprivate* d;
};

/**
 * Prints an "\n".
 * @param s the debug stream to write to
 * @return the debug stream (@p s)
 */
inline odbgstream& endl( odbgstream &s) { s << "\n"; return s; }
/**
 * Flushes the stream.
 * @param s the debug stream to write to
 * @return the debug stream (@p s)
 */
inline odbgstream& flush( odbgstream &s) { s.flush(); return s; }

odbgstream &perror( odbgstream &s);

/**
 * ondbgstream is a dummy variant of @ref odbgstream. All functions do
 * nothing.
 * @see ondDebug()
 */
class ondbgstream {
 public:
  /// Empty constructor.
    ondbgstream() {}
    ~ondbgstream() {}
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(short int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(unsigned short int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(char )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(unsigned char )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(int )  { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(unsigned int )  { return *this; }
    /**
     * Does nothing.
     */
    void flush() {}
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(const QString& ) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(const QCString& ) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &operator<<(const char *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator<<(const void *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator<<(void *) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator<<(double) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator<<(long) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator<<(unsigned long) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream& operator << (QWidget*) { return *this; }
    /**
     * Does nothing.
     * @return this stream
     */
    ondbgstream &form(const char *, ...) { return *this; }

    ondbgstream& operator<<( const QDateTime& ) { return *this; }
    ondbgstream& operator<<( const QDate&     ) { return *this; }
    ondbgstream& operator<<( const QTime&     ) { return *this; }
    ondbgstream& operator<<( const QPoint & )  { return *this; }
    ondbgstream& operator<<( const QSize & )  { return *this; }
    ondbgstream& operator<<( const QRect & )  { return *this; }
    ondbgstream& operator<<( const QRegion & ) { return *this; }
    ondbgstream& operator<<( const QStringList & ) { return *this; }
    ondbgstream& operator<<( const QColor & ) { return *this; }
    ondbgstream& operator<<( const QBrush & ) { return *this; }
    
private:
    class Private;
    Private *d;
};

/*======================================================================================
 * related functions
 *======================================================================================*/

/**
 * Does nothing.
 * @param a stream
 * @return the given @p s
 */
inline ondbgstream& endl( ondbgstream & s) { return s; }
/**
 * Does nothing.
 * @param a stream
 * @return the given @p s
 */
inline ondbgstream& flush( ondbgstream & s) { return s; }
inline ondbgstream& perror( ondbgstream & s) { return s; }

/**
 * Returns a debug stream. You can use it to print debug
 * information.
 * @param area an id to identify the output, 0 for default
 */
odbgstream odDebug(int area = 0);
odbgstream odDebug(bool cond, int area = 0);
/**
 * Returns a backtrace.
 * @param levels the number of levels (-1 for unlimited) of the backtrace
 * @return a backtrace
 */
QString odBacktrace(int levels = -1);
/**
 * Returns a dummy debug stream. The stream does not print anything.
 * @param area an id to identify the output, 0 for default
 * @see odDebug()
 */
inline ondbgstream ondDebug(int = 0) { return ondbgstream(); }
inline ondbgstream ondDebug(bool , int  = 0) { return ondbgstream(); }
inline QString ondBacktrace() { return QString::null; }
inline QString ondBacktrace(int) { return QString::null; }

/**
 * Returns a warning stream. You can use it to print warning
 * information.
 * @param area an id to identify the output, 0 for default
 */
odbgstream odWarning(int area = 0);
odbgstream odWarning(bool cond, int area = 0);
/**
 * Returns an error stream. You can use it to print error
 * information.
 * @param area an id to identify the output, 0 for default
 */
odbgstream odError(int area = 0);
odbgstream odError(bool cond, int area = 0);
/**
 * Returns a fatal error stream. You can use it to print fatal error
 * information.
 * @param area an id to identify the output, 0 for default
 */
odbgstream odFatal(int area = 0);
odbgstream odFatal(bool cond, int area = 0);

/**
 * Deletes the odebugrc cache and therefore forces KDebug to reread the
 * config file
 */
void odClearDebugConfig();

#ifdef OPIE_NO_DEBUG
#define odDebug ondDebug
#define odBacktrace ondBacktrace
#endif

#endif

}
}
