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
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "taskeditoralarms.h"

#include <opie2/opimnotifymanager.h>
#include <opie2/otimepicker.h>

#include <qpe/datebookmonth.h>
#include <qpe/resource.h>

#include <qlistview.h>
#include <qlayout.h>


using namespace Opie::Ui;
class AlarmItem : public QListViewItem {
public:
    AlarmItem( QListView*, const OPimAlarm& );
    ~AlarmItem();

    OPimAlarm alarm()const;
    void setAlarm( const OPimAlarm& );
private:
    QDateTime m_dt;
    int m_type;
};
AlarmItem::AlarmItem( QListView* view, const OPimAlarm& dt)
    : QListViewItem(view) {
    setAlarm( dt );
}
void AlarmItem::setAlarm( const OPimAlarm& dt ) {
    m_dt = dt.dateTime();
    m_type = dt.sound();
    setText( 0, TimeString::dateString( m_dt.date() ) );
    setText( 1, TimeString::timeString( m_dt.time() ) );
    setText( 2, m_type == 0 ? QObject::tr("silent") : QObject::tr("loud") );
}
AlarmItem::~AlarmItem() {
}
OPimAlarm AlarmItem::alarm()const{
    OPimAlarm al( m_type, m_dt );

    return al;
}

TaskEditorAlarms::TaskEditorAlarms( QWidget* parent,  int, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    m_date = m_type = m_time = 0;
    QGridLayout *layout = new QGridLayout( this, 2, 2, 4, 4 );

    lstAlarms = new QListView( this );
    lstAlarms->addColumn( tr("Date") );
    lstAlarms->addColumn( tr("Time") );
    lstAlarms->addColumn( tr("Type") );

    connect( lstAlarms, SIGNAL(clicked(QListViewItem*,const QPoint&,int) ),
             this, SLOT(inlineEdit(QListViewItem*,const QPoint&,int) ) );

    layout->addMultiCellWidget( lstAlarms, 0, 0, 0, 2 );

    QPushButton *btn = new QPushButton( Resource::loadPixmap( "new" ), tr( "New" ), this );
    //QWhatsThis::add( btn, tr( "Click here to add a new transaction." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotNew() ) );
    layout->addWidget( btn, 1, 0 );
/* use when we've reminders too */
#if 0
    btn = new QPushButton( Resource::loadPixmap( "edit" ), tr( "Edit" ), this );
    //QWhatsThis::add( btn, tr( "Select a transaction and then click here to edit it." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotEdit() ) );
    layout->addWidget( btn, 1, 1 );
#endif

    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), this );
    //QWhatsThis::add( btn, tr( "Select a checkbook and then click here to delete it." ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotDelete() ) );
    layout->addWidget( btn, 1, 2 );
}

TaskEditorAlarms::~TaskEditorAlarms(){
}

void TaskEditorAlarms::slotNew(){
    (void)new AlarmItem(lstAlarms, OPimAlarm(0, QDateTime::currentDateTime() ) );
}

void TaskEditorAlarms::slotEdit(){
}

void TaskEditorAlarms::slotDelete(){
    QListViewItem* item = lstAlarms->currentItem();
    if (!item) return;

    lstAlarms->takeItem( item ); delete item;


}

void TaskEditorAlarms::load( const OPimTodo& todo) {
    lstAlarms->clear();
    if (!todo.hasNotifiers() ) return;

    OPimNotifyManager::Alarms als = todo.notifiers().alarms();

    if (als.isEmpty() ) return;

    OPimNotifyManager::Alarms::Iterator it = als.begin();
    for ( ; it != als.end(); ++it )
        (void)new AlarmItem( lstAlarms, (*it) );


}
void TaskEditorAlarms::save( OPimTodo& todo ) {
    if (lstAlarms->childCount() <= 0 ) return;

    OPimNotifyManager::Alarms alarms;

    for ( QListViewItem* item = lstAlarms->firstChild(); item; item = item->nextSibling() ) {
        AlarmItem *alItem = static_cast<AlarmItem*>(item);
        alarms.append( alItem->alarm() );
    }

    OPimNotifyManager& manager = todo.notifiers();
    manager.setAlarms( alarms );
}
void TaskEditorAlarms::inlineEdit( QListViewItem* alarm, const QPoint& p, int col ) {
    if (!alarm) return;

    AlarmItem* item = static_cast<AlarmItem*>(alarm);
    switch( col ) {
        // date
    case 0:
        return inlineSetDate( item, p );
        // time
    case 1:
        return inlineSetTime( item );
        // type
    case 2:
        return inlineSetType( item, p );
    }
}
void TaskEditorAlarms::inlineSetDate( AlarmItem* item, const QPoint& p ) {
    QPopupMenu* pop = popup( 0 );
    m_dbMonth->setDate( item->alarm().dateTime().date() );
    pop->exec(p);

    OPimAlarm al = item->alarm();
    QDateTime dt = al.dateTime();
    dt.setDate( m_dbMonth->selectedDate() );
    al.setDateTime( dt );
    item->setAlarm( al );
}
void TaskEditorAlarms::inlineSetType( AlarmItem* item, const QPoint& p ) {
    int type;
    QPopupMenu* pop = popup( 2 );
    switch( pop->exec(p) ) {
    case 10:
        type = 1;
        break;
    case 20:
    default:
        type = 0;
    }
    OPimAlarm al = item->alarm();
    al.setSound( type );
    item->setAlarm( al );
}
void TaskEditorAlarms::inlineSetTime( AlarmItem* item ) {
    OPimAlarm al = item->alarm();
    QDateTime dt = al.dateTime();

    OTimePickerDialog dialog;
    dialog.setTime( dt.time() );
    if ( dialog.exec() == QDialog::Accepted ) {
        dt.setTime( dialog.time() );
        al.setDateTime( dt );
        item->setAlarm( al );
    }
}
QPopupMenu* TaskEditorAlarms::popup( int column ) {
    QPopupMenu* pop = 0;
    switch( column ) {
    case 0:{
        if (!m_date) {
            m_date = new QPopupMenu(this);
            m_dbMonth = new DateBookMonth(m_date, 0, TRUE);
            m_date->insertItem(m_dbMonth);
        }
        pop = m_date;
    }
        break;
    case 1:
        break;
    case 2:{
        if (!m_type) {
            m_type = new QPopupMenu(this);
            m_type->insertItem( QObject::tr("loud"), 10 );
            m_type->insertItem( QObject::tr("silent"), 20 );
        }
        pop = m_type;
    }
        break;
    default:
        break;
    }
    return pop;
}
