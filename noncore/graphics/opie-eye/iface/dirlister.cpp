#include "dirlister.h"

#include <qdir.h>

PDirLister::PDirLister( const char* name )
    : QObject( 0, name )
{}

PDirLister::~PDirLister()
{}

/**
 * Change dir one level up. The default implementation
 * is to use QDir to change the dir.
 * If you've a flat filesystem return \par path immediately.
 */
QString PDirLister::dirUp( const QString& path )const {
    QDir dir( path );
    dir.cdUp();

    return dir.absPath();
}
