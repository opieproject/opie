/*
                             This file is part of the Opie Project
                             (C) 2003 Michael 'Mickey' Lauer (mickey@tm.informatik.uni-frankfurt.de)
                             (C) 2002 Holger Freyther (freyther@kde.org)
                             (C) 1997 Matthias Kalle Dalheimer (kalle@kde.org)
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

// Include this header without OPIE_NO_DEBUG defined to avoid having the oDebugInfo
// functions inlined to noops (which would then conflict with their definition here).

#include <opie2/odebug.h>

#ifdef OPIE_NO_DEBUG
#undef odDebug
#undef odBacktrace
#endif

/* OPIE */

#include <opie2/oapplication.h>
#include <opie2/oglobalsettings.h>
#include <opie2/oconfig.h>

/* QT */

#include <qfile.h>
#include <qmessagebox.h>
#include <qsocketdevice.h>

/* UNIX */

#include <stdlib.h>     // abort
#include <unistd.h>     // getpid
#include <stdarg.h>     // vararg stuff
#include <ctype.h>      // isprint
#include <syslog.h>
#include <errno.h>
#include <string.h>

#ifndef OPIE_NO_BACKTRACE
#include <execinfo.h>
#endif

namespace Opie {
namespace Core {
namespace Internal {
/*======================================================================================
 * debug levels
 *======================================================================================*/

enum DebugLevels {
    ODEBUG_INFO     = 0,
    ODEBUG_WARN     = 1,
    ODEBUG_ERROR    = 2,
    ODEBUG_FATAL    = 3
};

/*======================================================================================
 * oDebug private data
 *======================================================================================*/

/*======================================================================================
 * the main debug function
 *======================================================================================*/



struct  DebugBackend {
    DebugBackend() : m_opened( false ), m_file( 0 ) ,m_port( -1 ),m_sock( 0 ) {
        m_outp = OGlobalSettings::debugMode();
    }
    ~DebugBackend() {
        delete m_file;
        delete m_sock;
    }
    void debug( unsigned short level, unsigned int, const QString& data );

private:
    void debugFile( const QString&, const QString& data );
    void debugMsgB( const QString&, const QString& data );
    void debugShel( const QString&, const QString& data );
    void debugSysl( int, const QString& );
    void debugSock( const QString&, const QString& data );
    QCString line( const QString&, const QString& data );
    bool m_opened : 1;
    QFile *m_file;
    QHostAddress m_addr;
    int m_port;
    QSocketDevice *m_sock;
    short m_outp;
};

void DebugBackend::debug(unsigned short level, unsigned int, const QString& data) {
    //qDebug( "oDebugBackend: Level=%d, Area=%d, Data=%s", level, area, data );

    // ML: OPIE doesn't use areacodes at the moment. See the KDE debug classes for an
    // ML: example use. I think it's not necessary to implement such a strategy here.
    // ML: Comments?

    int priority = 0;
    QString caption;
    QString lev;
    switch( level )
    {
        case ODEBUG_INFO: lev = "(Info)"; caption = "Info"; priority = LOG_INFO; break;
        case ODEBUG_WARN: lev = "(Warn)"; caption = "Warning"; priority = LOG_WARNING; break;
        case ODEBUG_FATAL: lev = "(Fatal)"; caption = "Fatal Error"; priority = LOG_CRIT; break;
        default: qDebug( "oDebugBackend: Warning: Unknown debug level! - defaulting to ODEBUG_ERROR." );
        case ODEBUG_ERROR: lev = "(Error)"; caption = "Error"; priority = LOG_ERR; break;
    }

    if (!oApp && (m_outp == 1)) {
        qDebug( "oDebugBackend: Warning: no oapplication object - can't use MsgBox" );
        m_outp = 2; // need an application object to use MsgBox
    }

    // gcc 2.9x is dumb and sucks... can you hear it?
    //QString areaName = (oApp) ? oApp->appName() : "<unknown>";
    QString areaName;
    if ( oApp ) areaName = oApp->appName();
    else areaName = "<unknown>";

    switch( m_outp ) {
    case ODEBUG_IGNORE:
        return;
    case ODEBUG_FILE:
        return debugFile( areaName, data );
    case ODEBUG_MSGBOX:
        return debugMsgB( areaName, data );
    case ODEBUG_STDERR:
        return debugShel( areaName,data );
    case ODEBUG_SYSLOG:
        return debugSysl( priority, data );
    case ODEBUG_SOCKET:
        return debugSock( areaName, data );
    }
}

inline void DebugBackend::debugFile(const QString& area, const QString& data) {
    /* something went wrong with the file don't bother.. */
    if ( m_opened && !m_file )
        return;
    else if ( !m_opened ) {
        m_opened = true;
        m_file = new QFile( OGlobalSettings::debugOutput() );
        if (!m_file->open( IO_WriteOnly | IO_Append ) ) {
            delete m_file; m_file = 0;
            qDebug( "ODebug: can't write to file '%s' (%s)", (const char*)OGlobalSettings::debugOutput(),
                    strerror(errno) );
            return;
        }
    }

    /* go to end of file */
    m_file->at( m_file->size() );
    QCString li = line( area, data );
    m_file->writeBlock(li.data(), li.length() );
}

void DebugBackend::debugMsgB( const QString& area, const QString& data ) {
    QMessageBox::warning( 0l, "("+area+")", data, ("Ok") );
}

void DebugBackend::debugShel( const QString& are, const QString& data ) {
    FILE *output = stderr;
    fprintf( output, "%s: %s", are.local8Bit().data(),
             data.local8Bit().data() );
}

void DebugBackend::debugSysl( int prio, const QString& data ) {
    ::syslog( prio, "%s", data.local8Bit().data() );
}

void DebugBackend::debugSock( const QString& are, const QString& data ) {
    if ( m_opened && !m_sock  )
        return;
    else if ( !m_opened ){
        m_opened = true;
        QString destination = OGlobalSettings::debugOutput();
        if ( destination && destination.find(":") != -1 ) {
            QString host = destination.left( destination.find(":") );
            m_port = destination.right( destination.length()-host.length()-1 ).toInt();
            m_addr.setAddress( host );
            m_sock = new QSocketDevice( QSocketDevice::Datagram );
        }else{
            m_sock = 0;
            return;
        }
    }

    QCString li = line( are, data );
    int result = m_sock->writeBlock(li.data(), li.length(), m_addr, m_port );
    if ( result == -1 ) {
         qDebug( "ODebug: can't send to address '"+ m_addr.toString() +":%d' (%s)",
                 m_port, strerror(errno) );
    }
}

QCString DebugBackend::line( const QString& area, const QString& data ) {
    QString str = area +":"+data;
    return str.local8Bit();
}

static  DebugBackend *backEnd = 0;
}
static void clean_up_routine() {
    qWarning( "Clean up Debug" );
    delete Internal::backEnd;
    Internal::backEnd = 0;
}
/*======================================================================================
 * odbgstream
 *======================================================================================*/

odbgstream& perror( odbgstream &s)
{
    return s << QString::fromLocal8Bit(strerror(errno));
}

odbgstream odDebug(int area)
{
    return odbgstream(area, Internal::ODEBUG_INFO);
}
odbgstream odDebug(bool cond, int area)
{
    if (cond) return odbgstream(area, Internal::ODEBUG_INFO);
    else return odbgstream(0, 0, false);
}

odbgstream odError(int area)
{
    return odbgstream("ERROR: ", area, Internal::ODEBUG_ERROR);
}

odbgstream odError(bool cond, int area)
{
    if (cond) return odbgstream("ERROR: ", area, Internal::ODEBUG_ERROR); else return odbgstream(0,0,false);
}

odbgstream odWarning(int area)
{
    return odbgstream("WARNING: ", area, Internal::ODEBUG_WARN);
}

odbgstream odWarning(bool cond, int area)
{
    if (cond) return odbgstream("WARNING: ", area, Internal::ODEBUG_WARN); else return odbgstream(0,0,false);
}

odbgstream odFatal(int area)
{
    return odbgstream("FATAL: ", area, Internal::ODEBUG_FATAL);
}

odbgstream odFatal(bool cond, int area)
{
    if (cond) return odbgstream("FATAL: ", area, Internal::ODEBUG_FATAL); else return odbgstream(0,0,false);
}

odbgstream::odbgstream(unsigned int _area, unsigned int _level, bool _print)
           :area(_area), level(_level),  print(_print)
{
}


odbgstream::odbgstream(const char * initialString, unsigned int _area, unsigned int _level, bool _print)
          :output(QString::fromLatin1(initialString)), area(_area), level(_level),  print(_print)
{
}


odbgstream::odbgstream(odbgstream &str)
           :output(str.output), area(str.area), level(str.level), print(str.print)
{
    str.output.truncate(0);
}


odbgstream::odbgstream(const odbgstream &str)
           :output(str.output), area(str.area), level(str.level), print(str.print)
{
}

odbgstream& odbgstream::operator<<(bool i)
{
    if (!print) return *this;
    output += QString::fromLatin1(i ? "true" : "false");
    return *this;
}


odbgstream& odbgstream::operator<<(short i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(unsigned short i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(unsigned char i)
{
    return operator<<( static_cast<char>( i ) );
}


odbgstream& odbgstream::operator<<(int i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(unsigned int i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(long i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(unsigned long i)
{
    if (!print) return *this;
    QString tmp; tmp.setNum(i); output += tmp;
    return *this;
}


odbgstream& odbgstream::operator<<(const QString& string)
{
    if (!print) return *this;
    output += string;
    if (output.at(output.length() -1 ) == '\n')
        flush();
    return *this;
}


odbgstream& odbgstream::operator<<(const char *string)
{
    if (!print) return *this;
    output += QString::fromUtf8(string);
    if (output.at(output.length() - 1) == '\n')
        flush();
    return *this;
}


odbgstream& odbgstream::operator<<(const QCString& string)
{
    *this << string.data();
    return *this;
}


odbgstream& odbgstream::operator<<(const void * p)
{
    form("%p", p);
    return *this;
}

odbgstream& odbgstream::operator<<(double d)
{
    QString tmp; tmp.setNum(d); output += tmp;
    return *this;
}

/*
odbgstream::odbgstream &form(const char *format, ...)
#ifdef __GNUC__
      __attribute__ ( ( format ( printf, 2, 3 ) ) )
#endif
     ;
*/

void odbgstream::flush()
{
    if ( output.isEmpty() || !print )
    {
        return;
    }
    else
    {
        if ( !Internal::backEnd ) {
            Internal::backEnd = new Internal::DebugBackend;
            qAddPostRoutine( clean_up_routine );
        }
        Internal::backEnd->debug( level, area, output );
        output = QString::null;
    }
}

odbgstream& odbgstream::form(const char *format, ...)
{
    char buf[4096];
    va_list arguments;
    va_start( arguments, format );
    buf[sizeof(buf)-1] = '\0';
    vsnprintf( buf, sizeof(buf)-1, format, arguments );
    va_end(arguments);
    *this << buf;
    return *this;
}

odbgstream::~odbgstream()
{
    if (!output.isEmpty())
    {
        fprintf(stderr, "ASSERT: debug output not ended with \\n\n");
        *this << "\n";
    }
}

odbgstream& odbgstream::operator<<(char ch)
{
    if (!print) return *this;
    if (!isprint(ch))
    {
        output += "\\x" + QString::number( static_cast<uint>( ch ) + 0x100, 16 ).right(2);
    }
    else
    {
        output += ch;
        if (ch == '\n') flush();
    }
    return *this;
}

odbgstream& odbgstream::operator<<( QWidget* widget )
{
    QString string, temp;
    // -----
    if(widget==0)
    {
        string=(QString)"[Null pointer]";
    } else
    {
        temp.setNum((ulong)widget, 16);
        string=(QString)"["+widget->className()+" pointer " + "(0x" + temp + ")";
        if(widget->name(0)==0)
        {
            string += " to unnamed widget, ";
        } else
        {
            string += (QString)" to widget " + widget->name() + ", ";
        }
        string += "geometry="
               + QString().setNum(widget->width())
               + "x"+QString().setNum(widget->height())
               + "+"+QString().setNum(widget->x())
               + "+"+QString().setNum(widget->y())
               + "]";
    }
    if (!print) return *this;

    output += string;
    if (output.at(output.length()-1) == '\n')
    {
        flush();
    }
    return *this;
}

/*
 * either use 'output' directly and do the flush if needed
 * or use the QString operator which calls the char* operator
 *
 */
odbgstream& odbgstream::operator<<( const QDateTime& time)
{
    *this << time.toString();
    return *this;
}


odbgstream& odbgstream::operator<<( const QDate& date)
{
    *this << date.toString();

    return *this;
}


odbgstream& odbgstream::operator<<( const QTime& time )
{
    *this << time.toString();
    return *this;
}


odbgstream& odbgstream::operator<<( const QPoint& p )
{
    *this << "(" << p.x() << ", " << p.y() << ")";
    return *this;
}


odbgstream& odbgstream::operator<<( const QSize& s )
{
    *this << "[" << s.width() << "x" << s.height() << "]";
    return *this;
}


odbgstream& odbgstream::operator<<( const QRect& r )
{
    *this << "[" << r.left() << ", " << r.top() << " - " << r.right() << ", " << r.bottom() << "]";
    return *this;
}


odbgstream& odbgstream::operator<<( const QRegion& reg )
{
    *this << "[ ";
    QArray<QRect>rs=reg.rects();
    for (uint i=0;i<rs.size();++i)
        *this << QString("[%1, %2 - %3, %4] ").arg(rs[i].left()).arg(rs[i].top()).arg(rs[i].right()).arg(rs[i].bottom() ) ;
    *this <<"]";
    return *this;
}


odbgstream& odbgstream::operator<<( const QStringList& l )
{
    *this << "(";
    *this << l.join(",");
    *this << ")";

    return *this;
}


odbgstream& odbgstream::operator<<( const QColor& c )
{
    if ( c.isValid() )
        *this << c.name();
    else
        *this << "(invalid/default)";
    return *this;
}


odbgstream& odbgstream::operator<<( const QBrush& b)
{
    static const char* const s_brushStyles[] = {
        "NoBrush", "SolidPattern", "Dense1Pattern", "Dense2Pattern", "Dense3Pattern",
        "Dense4Pattern", "Dense5Pattern", "Dense6Pattern", "Dense7Pattern",
        "HorPattern", "VerPattern", "CrossPattern", "BDiagPattern", "FDiagPattern",
        "DiagCrossPattern" };

    *this <<"[ style: ";
    *this <<s_brushStyles[ b.style() ];
    *this <<" color: ";
    // can't use operator<<(str, b.color()) because that terminates a odbgstream (flushes)
    if ( b.color().isValid() )
        *this <<b.color().name() ;
    else
        *this <<"(invalid/default)";
    if ( b.pixmap() )
        *this <<" has a pixmap";
    *this <<" ]";
    return *this;
}



QString odBacktrace( int levels )
{
    QString s;
#ifndef OPIE_NO_BACKTRACE
    void* trace[256];
    int n = backtrace(trace, 256);
    char** strings = backtrace_symbols (trace, n);

    if ( levels != -1 )
        n = QMIN( n, levels );
    s = "[\n";

    for (int i = 0; i < n; ++i)
        s += QString::number(i) +
             QString::fromLatin1(": ") +
             QString::fromLatin1(strings[i]) + QString::fromLatin1("\n");
    s += "]\n";
    free (strings);
#endif
    return s;
}

void odClearDebugConfig()
{
    /*
    delete oDebug_data->config;
    oDebug_data->config = 0;
    */
}


#ifdef OPIE_NO_DEBUG
#define odDebug ondDebug
#define odBacktrace ondBacktrace
#endif

}
}
