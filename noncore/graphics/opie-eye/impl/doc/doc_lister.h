/*
 * GPLv2 zecke@handhelds.org
 */

#ifndef DOC_LISTER_INTERFACE_LISTER_H
#define DOC_LISTER_INTERFACE_LISTER_H

#include <iface/dirlister.h>

#include <qstring.h>
#include <qmap.h>

class Config;
class Doc_DirLister : public PDirLister {
    Q_OBJECT
public:
    Doc_DirLister();
    virtual ~Doc_DirLister(){}

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
    QMap<QString,QString> m_namemap,m_filemap;
    QStringList m_out;
protected slots:
    virtual void slotFullInfo(const QString&, const QString&);
    virtual void slotThumbInfo(const QString&, const QString&);
    virtual void slotThumbNail(const QString&, const QPixmap&);
};

#endif
