/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#include "omodalhelper.h"

/* QT */
#include <qpushbutton.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>

/* Signal handling */
OModalHelperSignal::OModalHelperSignal( OModalHelperBase* base, QObject* parent )
        : QObject( parent, "OModal Helper Signal" ), m_base( base )
{}

OModalHelperSignal::~OModalHelperSignal()
{
    /* special the ancestor deletes its creator */
    delete m_base;
}


/* Helper Controler */
/*
 * the dialogs signal will be slotted here
 * and we will call into m_base
 */
OModalHelperControler::OModalHelperControler( OModalHelperBase* base, QObject* parent )
        : QObject(parent, "OModal Helper Controler" ), m_base( base ), m_dia( 0 ), m_id( -1 )
{}

TransactionID OModalHelperControler::transactionID()const
{
    return m_id;
}

void OModalHelperControler::setTransactionID( TransactionID id )
{
    m_dia = 0;
    m_id = id;
}

QDialog* OModalHelperControler::dialog()const
{
    return m_dia;
}

/*
 * If we're in the New mode we will map the QDialog
 * to the TransactionID
 */
void OModalHelperControler::done( int result )
{
    if ( sender() && !sender()->isA("OModalQueuedDialog") )
        m_dia = static_cast<QDialog*>( sender() );

    m_base->done( result, m_id );
}

void OModalHelperControler::next()
{
    m_base->next( m_id );
}

void OModalHelperControler::prev()
{
    m_base->prev( m_id );
}

/* The Queued Dialog inclusive QueuedBar */
struct OModalQueueBar : public QHBox
{
    QPushButton* next;
    QPushButton* prev;
    QLabel     * label;

    OModalQueueBar( QWidget* parent );
    void setText( const QString& str );
};

OModalQueueBar::OModalQueueBar( QWidget* parent )
        : QWidget( parent, "OModal Queue Bar" )
{
    prev = new QPushButton( this );
    prev->setText( OModalQueuedDialog::tr("Prev") );

    label = new QLabel(this);

    next = new QPushButton( this );
    next->setText( OModalQueuedDialog::tr("Next") );
}

void OModalQueueBar::setText( const QString& str )
{
    label->setText( str );
}


OModalQueuedDialog::OModalQueuedDialog( QDialog* mainWidget )
        : QDialog(0, "OModal Queued Dialog" )
{
    QVBoxLayout *lay = new QVBoxLayout( this );

    m_bar = new OModalQueueBar( this );
    lay->addWidget( m_bar );

    m_center = mainWidget;
    m_center->reparent(this, 0, QPoint(0, 0) );
    lay->addWidget( m_center );


    connect(m_bar->next, SIGNAL(clicked() ), this,
            SIGNAL(next() ) );
    connect(m_bar->prev, SIGNAL(clicked() ), this,
            SIGNAL(prev() ) );

}

OModalQueuedDialog::~OModalQueuedDialog()
{}

QDialog* OModalQueuedDialog::centerDialog()const
{
    return m_center;
}

void OModalQueuedDialog::setQueueBarEnabled( bool b)
{
    /* in Qt3 use setEnabled( bool ) */
    if (b)
        m_bar->show();
    else
        m_bar->hide();
}

void OModalQueuedDialog::setRecord( int record, int count )
{
    if (!record && !count )
    {
        hide();
        return;
    }
    else
        show();

    if ( count > 1 )
        m_bar->show();
    else
        m_bar->hide();

    m_bar->setText( tr("Editing record %1 out of %2",
                       "Shows the current edited record out of an array of records").arg( record ). arg( count ) );
}
