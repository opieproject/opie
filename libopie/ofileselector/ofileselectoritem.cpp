#include "ofileselectoritem.h"

OFileSelectorItem::OFileSelectorItem( QListView*view,
                                      const QPixmap& pix,
                                      const QString& path,
                                      const QString& date,
                                      const QString& size,
                                      const QString& dir,
                                      bool isLocked,
                                      bool isDir )
    : QListViewItem( view )
{
    setPixmap( 0, pix );
    setText( 1, path );
    setText( 2, size );
    setText( 3, date );
    m_dir = isDir;
    m_locked = isLocked;
    m_dirStr = dir;
}
OFileSelectorItem::~OFileSelectorItem() {
}
bool OFileSelectorItem::isLocked()const {
    return m_locked;
}
QString OFileSelectorItem::directory()const {
    return m_dirStr;
}
bool OFileSelectorItem::isDir()const {
    return m_dir;
}
QString OFileSelectorItem::path() const {
    return text(1);
}
QString OFileSelectorItem::key( int id, bool ) {
    QString ke;

    if( id == 0 || id == 1 ){ // name
	if( m_dir ){
            ke.append("0" );
            ke.append( text(1) );
	}else{
            ke.append("1" );
            ke.append( text(1) );
	}
    }else if( id == 2 ){ // size
	return text(2);
    }else if( id == 3 ){ // date
	return text(3);
    }

    return ke;
}
