/*
 * GPLv2 zecke@handhelds.org
 */

#ifndef DCIM_LISTER_H
#define DCIM_LISTER_H

#include <iface/dirlister.h>

#include <qmap.h>

class Config;

/**
 * The DirLister is responsible for
 * telling the GUI about files and folders
 * and to load them.
 * The DCIM is a special FileSystem lister
 * in the that it starts with an overview
 * of available 'Cameras'. It looks via 'Storage'
 * for paths including a 'dcim' directory.
 * And offers these as folders.
 * Then it only lists Images. It tries to use
 * the 'preview' directory to generate a thumbnail
 * but will use the full image for the QImage
 * call.
 *
 */
class DCIM_DirLister : public PDirLister {
    Q_OBJECT
public:
    enum ListMode{
        ListingUnknown = -1,
        ListingStart=1,  /* give an overview over files   */
        ListingFolder, /* give access to albums         */
	ListingFiles,   /* list the content of the album */
	ListingReFolder
    };
    DCIM_DirLister();
    ~DCIM_DirLister();

    QString defaultPath()const;
    QString setStartPath( const QString& );

    QString currentPath()const;
    QStringList folders()const;
    QStringList files()  const;

    void deleteImage( const QString& );

    void thumbNail( const QString&, int, int );
    QImage image( const QString&, Factor, int );
    void imageInfo( const QString& );
    void fullImageInfo( const QString& );


    virtual QString nameToFname( const QString& )const;
    virtual QString dirUp(const QString& )const;

private:
    QStringList findCameras()const;
    QStringList findAlbums ()const;
    QStringList findImages ()const;

private:
    mutable QString  m_path;
    mutable ListMode m_mode;
    mutable QMap<QString, QString> m_map;
};

#endif
