/*
 * GPLv2 zecke@handhelds.org
 */

#include "slaveiface.h"

static SlaveMap* _slaveMap = 0;
SlaveMap* slaveMap() {
    if ( !_slaveMap )
        _slaveMap = new SlaveMap;
    return _slaveMap;
}

SlaveInterface::SlaveInterface( const QStringList& image )
    : m_list( image )
{

}

SlaveInterface::~SlaveInterface() {

}

QStringList SlaveInterface::imageFormats()const {
    return m_list;
}
