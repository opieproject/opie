
#include "rfcommassigndialogitem.h"

#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qgroupbox.h>

using namespace OpieTooth;


RfcommDialogItem::RfcommDialogItem( QWidget* parent,  const char* name, WFlags fl )
    : RfcommDialogItemBase( parent, name, fl ) {

}

RfcommDialogItem::~RfcommDialogItem()  {
}


int RfcommDialogItem::ident()  {
    return m_ident;
}

QString RfcommDialogItem::mac()  {
    return m_macAddress->text();
}

int RfcommDialogItem::channel()  {
    return m_channelDropdown->currentItem();
}

QString RfcommDialogItem::comment()  {
    return m_commentLine->text();
}

void RfcommDialogItem::setIdent( int ident )  {
    m_ident = ident;
    m_identLabel->setTitle( QString( "rfcomm%1").arg( ident ) );
}

void RfcommDialogItem::setMac( const QString &mac )  {
    m_macAddress->setText( mac );
}

void RfcommDialogItem::setChannel( int channel )  {
    m_channelDropdown->setCurrentItem( channel );
}

void RfcommDialogItem::setComment( const QString &comment ) {
    m_commentLine->setText( comment );
}


