/*
                             This file is part of the Opie Project
                             Copyright (C) 2003 Michael 'Mickey' Lauer <mickey@Vanille.de>
                             Copyright (C) 2004 Holger 'zecke' Freyther <zecke@handhelds.org>
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

#ifndef OGLOBAL_H
#define OGLOBAL_H


#include <opie2/oconfig.h>

#ifndef private
#define HACK_DEFINED
#define private protected
#endif
#include <qpe/global.h>
#ifdef HACK_DEFINED
#undef private
#endif

#include <sys/types.h>

//FIXME Is it wise or even necessary to inherit OGlobal from Global?
//      once we totally skip libqpe it should ideally swallow Global -zecke
//      You're right. I deleted global as the base class. -mickeyl


class QFile;
class QString;
class DateFormat;
/**
 *\brief OGlobal contains a list of generic functions
 *
 * The class OGlobal contains small utility functions
 * which might be useful for other applications to use. It features access
 * to the global device config and specialized functions to get information
 * out of this config like Weekstart or Owner name.
 *
 * @todo ODP implement the things from Global which are good
 * @author mickey,alwin,zecke
 * @version 0.1
 */
class OGlobal : public Global
{
public:

    //  how do they relate to our Document Idea
    /** @name Document System related functions
     *
     */
    //@{
    static bool isAppLnkFileName( const QString& str );
    static bool isDocumentFileName( const QString& file );
    //@}

    /** @name File Operations
     *  File operations provided by OGlobal
     */
    //@{
    static QString tempDirPath();
    static QString homeDirPath();
    static QString tempFileName( const QString& );
    static bool renameFile( const QString& from, const QString& to );
    static bool truncateFile( QFile &f, off_t size );
    //@}


    static QString generateUuid();

    /** @name Convert Content
     * Convert Content of a QByteArray
     */
    //@{
    static QByteArray encodeBase64(const QByteArray&, bool insertLF = false );
    static QByteArray decodeBase64(const QByteArray& );
    //@}

    //FIXME Do we want to put that into OApplication as in KApplication? -zecke
    //      We already have a per-application config in OApplication
    //      ( accessed through oApp->config() ), but this one is the global one! -mickeyl
    /** @name Config and Owner related Information
     *
     */
    //@{
    static OConfig* config();
    static QString ownerName();
    static bool weekStartsOnMonday();
    static bool useAMPM();
#ifdef ODP
#error "Fix dateFormat"
   /**
    * For Qt3/Qt4 we can use QDate::toString(OGlobal::dateFormat)
    * See if we need to use the function with String in it
    * Anyway this is the future
    * for now still use TimeString!
    */
#endif
    static DateFormat dateFormat();
    static void setDateFormat( const DateFormat& );


    static void setWeekStartsOnMonday( bool );
    static void setUseAMPM( bool );
    //@}

   //@{
    static Global::Command* builtinCommands();
    static QGuardedPtr<QWidget>* builtinRunning();
   //@}

private:
    static OConfig* _config;
};

#endif // OGLOBAL_H
