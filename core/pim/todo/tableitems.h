/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef OPIE_TODO_TABLE_ITEMS_H
#define OPIE_TODO_TABLE_ITEMS_H

#include <qarray.h>
#include <qguardedptr.h>
#include <qcombobox.h>
#include <qtable.h>

#include <qpe/stringutil.h>

#include <opie/ocheckitem.h>
#include <opie/otodo.h>


namespace Todo {

    /**
     * We need to reimplement OCheckItem here
     * we need to update on change
     * You can check a checkItem
     */
    class CheckItem : public OCheckItem {
    public:
        CheckItem( QTable* t, const QString& sortKey,
                   int uid,
                   const QArray<int>& );
        ~CheckItem();
        virtual void setChecked( bool b );
        virtual void toggle();
        int uid()const;
        QArray<int> cats();
    private:
        int m_uid;
        QArray<int> m_cat;
    };

    /**
     * creates a QComboBox if neccessary
     * to edit inline
     */
    class ComboItem : public QTableItem {
    public:
        ComboItem( QTable* t, EditType et );
        ~ComboItem();
        QWidget* createEditor()const;
        void setContentFromEditor( QWidget* w );
        void setText( const QString& s );
        int alignment() const;

        QString text()const;

    private:
        QGuardedPtr<QComboBox> m_cb;
    };

    /* implement inline */
    inline int ComboItem::alignment()const {
        return Qt::AlignCenter;
    }

    /**
     * TodoTextItem builds a better specialised
     * sortkey
     */
    class TodoTextItem : public QTableItem {
    public:
        TodoTextItem( QTable* t, const QString& str );
        ~TodoTextItem();
        QString key()const;
    };


    inline QString TodoTextItem::key() const {
        return Qtopia::buildSortKey( text() );
    }

    class DueTextItem : public QTableItem {
    public:
        DueTextItem( QTable* t, const OTodo& );
        ~DueTextItem();

        QString key()const;
        void setToDoEvent( const OTodo& ev );
        void setCompleted( bool comp );
        void paint( QPainter *p, const QColorGroup &cg,
                    const QRect& cr, bool selected );
    private:
        int m_off;
        bool m_hasDate:1;
        bool m_completed:1;
    };


};

#endif
