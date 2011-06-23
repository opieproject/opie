
#include "btdeviceitem.h"

using namespace OpieTooth;


BTDeviceItem::BTDeviceItem( QListView* parent,  const Opie::Bluez::DeviceRecord& dev )
    : BTListItem( parent )
{
    setText( 0, dev.name() );
    m_device = dev;
}

BTDeviceItem::~BTDeviceItem()
{
}

Opie::Bluez::DeviceRecord BTDeviceItem::remoteDevice() const
{
    return m_device;
}

QString BTDeviceItem::mac() const
{
    return m_device.mac();
}

QString BTDeviceItem::name() const
{
    return m_device.name();
}

QString BTDeviceItem::type() const
{
    return QString::fromLatin1("device");
}

int BTDeviceItem::typeId() const
{
    return Device;
}
