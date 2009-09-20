/*
                            This file is part of the Opie Project
                            
             =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
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

#ifndef OPIMMEMO_H
#define OPIMMEMO_H

/* OPIE */
#include <opie2/opimrecord.h>
#include <qpe/recordfields.h>
#include <qpe/palmtopuidgen.h>

/* QT */
#include <qarray.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>

namespace Opie
{

class OPimState;
class OPimRecurrence;
class OPimMaintainer;
class OPimNotifyManager;
class OPimMemo : public OPimRecord
{
  public:
    typedef QValueList<OPimMemo> ValueList;
    enum RecordFields {
        Uid = Qtopia::UID_ID,
        Category = Qtopia::CATEGORY_ID,
        Text
    };
  public:
    OPimMemo();
      
    OPimMemo( const QMap<int, QString> &map );

    /* Constructs a new memo
       @param text The text of the memo
       @param category The categories it belongs to, if any
       @param uid what is the UUID of this memo
    **/
    OPimMemo( const QString &text,
           const QStringList &category = QStringList(),
           int uid = 0 /*empty*/ );

    /* Constructs a new memo
       @param text The text of the memo
       @param category The categories it belongs to, if any ( uids )
       @param uid what is the UUID of this memo
    **/
    OPimMemo( const QString &text,
           const QArray<int>& category,
           int uid = 0 /*empty*/ );

    /** Copy c'tor
     *
     */
    OPimMemo( const OPimMemo & );

    /**
     *destructor
     */
    ~OPimMemo();

    /**
     * The text of the memo
     */
    QString text() const;

    /**
     * The description of the memo
     */
    QString description() const;

    /**
     * @reimplemented
     * Return this todoevent in a RichText formatted QString
     */
    QString toRichText() const;

    /**
     * reimplementations
     */
    QString type() const;
    QString toShortText() const;
    QString recordField( int id ) const;

    /**
     * toMap puts all data into the map. int relates
     * to ToDoEvent RecordFields enum
     */
    QMap<int, QString> toMap() const;

    /**
     * fromMap sets the data from a map. int relates
     * to ToDoEvent RecordFields enum
     */
    void fromMap( const QMap<int, QString>& map );

    /**
     * set the text
     */
    void setText( const QString& text );

    virtual bool match( const QRegExp &r ) const;

    bool operator<( const OPimMemo &memo ) const;
    bool operator<=( const OPimMemo &memo ) const;
    bool operator!=( const OPimMemo &memo ) const;
    bool operator>( const OPimMemo &memo ) const;
    bool operator>=( const OPimMemo &memo ) const;
    bool operator==( const OPimMemo &memo ) const;
    OPimMemo &operator=( const OPimMemo &memo );

    //@{
    int rtti() const;
    static OPimMemo* safeCast( const OPimRecord* );
    //@}

    ChangeAction action() const;
    void setAction( ChangeAction action );

  private:
    class OPimMemoPrivate;
    struct OPimMemoData;

    void deref();
    inline void changeOrModify();
    void copy( OPimMemoData* src, OPimMemoData* dest );
    OPimMemoPrivate *d;
    OPimMemoData *data;

};


inline bool OPimMemo::operator!=( const OPimMemo &memo ) const
{
    return !( *this == memo );
}

}

#endif
