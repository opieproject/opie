/*
 * GPLv2 zecke@handhelds.org
 */

#ifndef DIR_LISTER_INTERFACE_LISTER_H
#define DIR_LISTER_INTERFACE_LISTER_H

#include <qdir.h>

#include <iface/dirlister.h>

class Config;
class Dir_DirLister : public PDirLister {
public:
    Dir_DirLister( bool );

    QString defaultPath()const;
    QString setStartPath( const QString& );
    QString currentPath()const;
    QStringList folders()const;
    QStringList files()const;

    void deleteImage( const QString& );
    void thumbNail( const QString&, int, int );
    QImage image( const QString&, Factor, int );
    void imageInfo( const QString& );
    void fullImageInfo( const QString& );

private:
    bool m_allFiles;
    QDir m_currentDir;
};

#endif
