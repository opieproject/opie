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
#include "tableview.h"

#include "tableitems.h"
#include <string.h>

#include <qobject.h>

using namespace Todo;

CheckItem::CheckItem( QTable* t,
                      const QString& sortKey,
                      int uid,
                      const QArray<int>& lis)
    : OCheckItem(t, sortKey), m_uid(uid ), m_cat( lis )
{
}
CheckItem::~CheckItem() {
}
void CheckItem::setChecked( bool b ) {
    OCheckItem::setChecked(b);
}
void CheckItem::toggle() {
    TableView* view = static_cast<TableView*>( table() );
    OTodo ev = view->find( view->current() );
    ev.setCompleted(!isChecked() );
    //view->updateFromTable( ev );

    OCheckItem::toggle();
    table()->updateCell( row(), col() );
}
int CheckItem::uid() const {
    return m_uid;
}
QArray<int> CheckItem::cats() {
    return m_cat;
}

/* ComboItem */
ComboItem::ComboItem( QTable* t, EditType et )
    : QTableItem( t, et, "3" ), m_cb(0)
{
    setReplaceable( FALSE );
}
ComboItem::~ComboItem() {

}
QWidget* ComboItem::createEditor()const {
    qWarning( "create editor");
    QString txt = text();

    ( (ComboItem*)this)-> m_cb = new QComboBox( table()->viewport() );

    m_cb->insertItem( "1" );
    m_cb->insertItem( "2" );
    m_cb->insertItem( "3" );
    m_cb->insertItem( "4" );
    m_cb->insertItem( "5" );
    m_cb->setCurrentItem( txt.toInt() - 1 );

    return m_cb;
}
void ComboItem::setContentFromEditor( QWidget* w) {
    TableView* view = static_cast<TableView*>( table() );
    OTodo ev = view->find( view->current() );

    if ( w->inherits( "QComboBox" ) )
        setText( ( (QComboBox*)w )->currentText() );
    else
        QTableItem::setContentFromEditor( w );

    ev.setPriority( text().toInt() );
    //view->updateFromTable( ev );
}
void ComboItem::setText( const QString& s ) {
    if ( m_cb )
        m_cb->setCurrentItem( s.toInt()-1 );

    QTableItem::setText( s );
}
QString ComboItem::text()const {
    if ( m_cb)
        return m_cb->currentText();

    return QTableItem::text();
}

/* TodoTextItem */
TodoTextItem::~TodoTextItem() {

}
TodoTextItem::TodoTextItem( QTable* t,
                            const QString& string )
    : QTableItem( t,  QTableItem::Never, string )
{}

/* DueTextItem */
DueTextItem::DueTextItem( QTable* t, const OTodo& ev)
    : QTableItem(t, Never, QString::null )
{
    setToDoEvent( ev );
}
DueTextItem::~DueTextItem() {

}
QString DueTextItem::key() const {
    QString key;

    if( m_hasDate ){
        if(m_off == 0 ){
            key.append("b");
        }else if( m_off > 0 ){
            key.append("c");
        }else if( m_off < 0 ){
            key.append("a");
        }
        key.append(QString::number(m_off ) );
    }else{
        key.append("d");
    }
    return key;
}
void DueTextItem::setCompleted( bool comp ) {
    m_completed = comp;
    table()->updateCell( row(), col() );
}
void DueTextItem::setToDoEvent( const OTodo& ev ) {
    m_hasDate = ev.hasDueDate();
    m_completed = ev.isCompleted();

    if( ev.hasDueDate() ){
        QDate today = QDate::currentDate();
        m_off = today.daysTo(ev.dueDate() );
        setText( QObject::tr( "%1 day(s)" ).arg( QString::number(m_off) ) );
    }else{
        setText("n.d." );
        m_off = 0;
    }
}
void DueTextItem::paint( QPainter* p, const QColorGroup &cg,
                         const QRect& cr, bool selected ) {
    QColorGroup cg2(cg);

    QColor text = cg.text();
    if( m_hasDate && !m_completed ){
        if( m_off < 0 ){
            cg2.setColor(QColorGroup::Text, QColor(red ) );
        }else if( m_off == 0 ){
            cg2.setColor(QColorGroup::Text, QColor(yellow) ); // orange isn't predefined
        }else if( m_off > 0){
            cg2.setColor(QColorGroup::Text, QColor(green ) );
        }
    }
    QTableItem::paint(p, cg2, cr, selected );
    /* restore default color */
    cg2.setColor(QColorGroup::Text, text );
}
