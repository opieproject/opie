/*
 * GPLv2 zecke@handhelds.org
 */

#ifndef DIR_LISTER_INTERFACE_LISTER_H
#define DIR_LISTER_INTERFACE_LISTER_H

#include <qdir.h>

#include <iface/dirlister.h>

class Config;
class Dir_DirLister : public PDirLister {
    Q_OBJECT
public:
    Dir_DirLister(bool,bool,int);
    virtual ~Dir_DirLister(){}

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
    virtual QString nameToFname(const QString&name)const;

private:
    bool m_allFiles:1;
    bool m_recursive:1;
    int m_recDepth;
    QDir m_currentDir;
    //! recursive listing.
    /*!
     * \param path this is the offset to the current path. eg. when currentDepth = 0 then it MUST empty
     */
    QStringList recFiles(const QString&path,int currentDepth)const;
    QString m_Filter;
};

#endif
