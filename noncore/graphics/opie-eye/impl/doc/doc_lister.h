/*
 * GPLv2 zecke@handhelds.org
 */

#ifndef DOC_LISTER_INTERFACE_LISTER_H
#define DOC_LISTER_INTERFACE_LISTER_H

#include <iface/dirlister.h>

#include <qpe/applnk.h>

#include <qstring.h>
#include <qmap.h>

class Config;
class AppLnk;
class QCopChannel;

class Doc_DirLister : public PDirLister {
    Q_OBJECT
public:
    Doc_DirLister();
    virtual ~Doc_DirLister();

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
    QString dirUp( const QString& )const;
    QWidget* widget(QWidget*parent);

private:
    int m_catFilter;
    bool matchCat(const AppLnk* app)const;
    bool m_docreads;
    DocLnkSet m_ds;

protected slots:
    virtual void slotFullInfo(const QString&, const QString&);
    virtual void slotThumbInfo(const QString&, const QString&);
    virtual void slotThumbNail(const QString&, const QPixmap&);
    virtual void showCategory(int);
    void systemMsg(const QCString &, const QByteArray &);
    QCopChannel *syschannel;
};

#endif
