/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#include <qpopupmenu.h>
#include <qtoolbar.h>

#include <qpe/resource.h>
#include <qpe/storage.h>


#include "filesystem.h"

PFileSystem::PFileSystem(  QToolBar* bar)
    : QToolButton( bar )
{
    setIconSet( Resource::loadIconSet( "cardmon/pcmcia" ) );

    m_pop = new QPopupMenu( this );
    connect( m_pop, SIGNAL( activated( int ) ),
             this, SLOT(slotSelectDir( int ) ) );

    m_storage = new StorageInfo();
    connect(m_storage, SIGNAL(disksChanged() ),
            this, SLOT( changed() ) );
    changed();

    setPopup( m_pop );
}

PFileSystem::~PFileSystem() {
    delete m_storage;
}


void PFileSystem::changed() {
    m_pop->clear();
    m_dev.clear();
    const QList<FileSystem> &fs = m_storage->fileSystems();
    QListIterator<FileSystem> it(fs );
    for ( ; it.current(); ++it ) {
        const QString disk = (*it)->name();
        const QString path = (*it)->path();
        m_dev.insert( disk, path );
        m_pop->insertItem( disk );
    }
}

void PFileSystem::slotSelectDir( int id ) {
    emit changeDir( m_dev[m_pop->text(id )] );
}
