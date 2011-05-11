
#include "btserviceitem.h"

using namespace OpieTooth;

BTServiceItem::BTServiceItem( QListViewItem* item,  const Opie::Bluez::OBluetoothServices& serv )
    : BTListItem( item )
{
    int port = serv.protocolDescriptorList().last().port();
    m_service = serv;
    setText(0, QObject::tr(serv.serviceName() ) );
    if (port > 0)
        setText(1, QString::number(port) );
}

BTServiceItem::~BTServiceItem()
{
}

QString BTServiceItem::type() const
{
    return QString::fromLatin1("service");
}

int BTServiceItem::typeId() const
{
    return Service;
}

Opie::Bluez::OBluetoothServices BTServiceItem::services() const
{
    return m_service;
}

int BTServiceItem::serviceId() const
{
    return m_service.recHandle();
}
