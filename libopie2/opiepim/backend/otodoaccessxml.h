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
#include <qintdict.h>

#include <opie2/otodoaccessbackend.h>
#include <opie2/opimio.h>

namespace Opie {

class OPimTodoAccessXML;


class OPimTodoXmlHandler : public OPimXmlHandler
{
public:
    OPimTodoXmlHandler( QAsciiDict<int> &dict, OPimTodoAccessXML &backend );
    void handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap );
protected:
    OPimTodoAccessXML &m_backend;
};


class OPimTodoAccessXML : public OPimTodoAccessBackend {
public:
    /**
     * fileName if Empty we will use the default path
     */
    OPimTodoAccessXML( const QString& appName,
                    const QString& fileName = QString::null );
    ~OPimTodoAccessXML();

    bool load();
    bool reload();
    bool save();

    bool write( OAbstractWriter &wr );

    QArray<int> allRecords()const;
    QArray<int> matchRegexp(const QRegExp &r) const;
    OPimTodo find( int uid )const;
    void clear();
    bool add( const OPimTodo& );
    bool remove( int uid );
    void removeAllCompleted();
    bool replace( const OPimTodo& );

    /* our functions */
    QArray<int> effectiveToDos( const QDate& start,
                                const QDate& end,
                                bool includeNoDates )const;
    QArray<int> overDue()const;

    friend void OPimTodoXmlHandler::handleItem( QMap<int, QString> &map, QMap<QString, QString> &extramap );
//@{
    UIDArray sorted( const UIDArray&, bool, int, int, const QArray<int>& )const;
//@}
private:
    void initDict( QAsciiDict<int> &dict ) const;
    inline void finalizeRecord( OPimTodo& todo );
    QString toString( const OPimTodo&, const QIntDict<QString> & )const;
    QString toString( const QArray<int>& ints ) const;
    QMap<int, OPimTodo> m_events;
    QString m_file;
    QString m_app;
    bool m_opened : 1;
    bool m_changed : 1;
    class OPimTodoAccessXMLPrivate;
    OPimTodoAccessXMLPrivate* d;
};

};

#endif
