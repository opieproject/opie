/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_DIR_LISTER_H
#define PHUNK_DIR_LISTER_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>


class PDirLister : public QObject {
    Q_OBJECT
public:
    enum Factor { Width, Height, None };

    PDirLister( const char* name );

    virtual QString defaultPath()const = 0;
    virtual QString setStartPath( const QString& ) = 0;
    virtual QString currentPath()const = 0;
    virtual QStringList folders()const = 0;
    virtual QStringList files()const = 0;
public slots:
    virtual void deleteImage( const QString& ) = 0;
    virtual void imageInfo( const QString&) = 0;
    virtual void fullImageInfo( const QString& ) = 0;
    virtual void thumbNail( const QString&, int max_wid, int max_h ) = 0;
    virtual QImage image(  const QString&, Factor, int max = 0) = 0;
    virtual QString nameToFname(const QString&name)const = 0;

signals:
    void sig_dirchanged();
    void sig_filechanged();
    void sig_start();
    void sig_end();
// If this app ever happens to get multithreaded...
    void sig_thumbInfo( const QString&, const QString& );
    void sig_fullInfo( const QString&, const QString& );
    void sig_thumbNail( const QString&, const QPixmap& );

protected:
    virtual ~PDirLister();
};

#endif
