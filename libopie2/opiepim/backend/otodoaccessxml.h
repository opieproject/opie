/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
#ifndef OPIE_TODO_ACCESS_XML_H
#define OPIE_TODO_ACCESS_XML_H

#include <qasciidict.h>
#include <qmap.h>

#include <opie2/otodoaccessbackend.h>

namespace Opie {
    class XMLElement;

class OTodoAccessXML : public OTodoAccessBackend {
public:
    /**
     * fileName if Empty we will use the default path
     */
    OTodoAccessXML( const QString& appName,
                    const QString& fileName = QString::null );
    ~OTodoAccessXML();

    bool load();
    bool reload();
    bool save();

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    QArray<int> queryByExample( const OTodo&, int querysettings, const QDateTime& d = QDateTime() );
    OTodo find( int uid )const;
    void clear();
    bool add( const OTodo& );
    bool remove( int uid );
    void removeAllCompleted();
    bool replace( const OTodo& );

    /* our functions */
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates );
    QArray<int> overDue();
    QArray<int> sorted( bool asc,  int sortOrder,
                        int sortFilter, int cat );
    QBitArray supports()const;
private:
    static QBitArray sup();
    void todo( QAsciiDict<int>*, OTodo&,const QCString&,const QString& );
    QString toString( const OTodo& )const;
    QString toString( const QArray<int>& ints ) const;
    QMap<int, OTodo> m_events;
    QString m_file;
    QString m_app;
    bool m_opened : 1;
    bool m_changed : 1;
    class OTodoAccessXMLPrivate;
    OTodoAccessXMLPrivate* d;
    int m_year, m_month, m_day;

};

};

#endif
