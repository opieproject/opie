/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <zecke@handhelds.org>
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
#include <qshared.h>

#include "smalltodo.h"

using namespace Todo;

struct SmallTodo::SmallTodoPrivate  : public QShared{

    SmallTodoPrivate() : QShared(), uid(-1) {};
    QString name;
    QStringList categories; // as real Names
    int uid;
    bool complete:1;
    QDate date;

    void deleteSelf() { delete this; };
};

SmallTodo::SmallTodo(int uid,
                     bool comp,
                     const QDate& date,
                     const QString& name,
                     const QStringList& cats) {
    d = new SmallTodoPrivate();
    d->name = name;
    d->uid = uid;
    d->categories = cats;
    d->complete = comp;
    d->date = date;
}
SmallTodo::SmallTodo( const SmallTodo& s ) : d(s.d) {
    d->ref();
}
SmallTodo::~SmallTodo() {
    /* deref and if last one delete */
    if ( d->deref() ) {
        d->deleteSelf();
    }
}
bool SmallTodo::operator==( const SmallTodo& todo ) {
    if ( d->complete != todo.d->complete ) return false;
    if ( d->name != todo.d->name ) return false;
    if ( d->uid != todo.d->uid ) return false;
    if ( d->categories != todo.d->categories ) return false;
    if ( d->date != todo.d->date ) return false;

    return true;
}
bool SmallTodo::operator==( const SmallTodo& todo ) const{
    if ( d->complete != todo.d->complete ) return false;
    if ( d->uid != todo.d->uid ) return false;
    if ( d->name != todo.d->name ) return false;
    if ( d->categories != todo.d->categories ) return false;
    if ( d->date != todo.d->date ) return false;

    return true;
}
SmallTodo &SmallTodo::operator=( const SmallTodo& todo ) {
    todo.d->ref();
    deref();

    d = todo.d;

    return *this;
}
void SmallTodo::deref() {
    if ( d->deref() ) {
        delete d;
        d = 0;
    }
}
QString SmallTodo::name() const {
    return d->name;
}
QStringList SmallTodo::categories()const {
    return d->categories;
}
int SmallTodo::uid()const {
    return d->uid;
}
bool SmallTodo::isCompleted()const {
    return d->complete;
}
QDate SmallTodo::date()const {
    return d->date;
}
