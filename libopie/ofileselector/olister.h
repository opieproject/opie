#ifndef OPIE_FILE_LISTER_H
#define OPIE_FILE_LISTER_H

#include <qfileinfo.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

class QComboBox;
class OPixmapProvider;
class OFileSelector;

class OListerCmbAccess;
/**
 * lister is something like KIO but very
 * very basic and currently only for
 * populating our views.
 * This is a base class which needs to be implemented.
 * @see OLocalLister for a filesystem based implementation
 */

class OLister {
public:
    OLister( OFileSelector* );
    virtual ~OLister();
    virtual void reparse(const QString& path) = 0;

    /**
     * return a list of available mimetypes
     */
    virtual QMap<QString, QStringList> mimeTypes( const QString& dir ) = 0;
    void setPixmapProvider( OPixmapProvider* );


    /* some way a slot */
    void fileSelected( const QString& dir, const QString& file, const QString& extra ) = 0;
    void changeDir( const QString& dir, const QString& file, const QString& extra ) = 0;
protected:
    /**
     * I hate too big classes
     * this is a way to group
     * access to a ComboBox
     * which might exist or
     * not in a secure way
     */
    OListerCmbAccess* comboBox();

    bool showFiles()const;
    bool showDirs()const;
    bool compliesMime( const QString& mime );
    void addFile( const QString& mine,
                  QFileInfo*,
                  const QString& extra = QString::null,
                  bool isSymlink = FALSE );

    void addFile( const QString& mine,
                  const QString& path,
                  const QString& file,
                  const QString& extra = QString::null,
                  bool isSymlink = FALSE );
    void addDir( const QString& mine,
                 QFileInfo*,
                 bool isSymlink = FALSE );
    void addDir( const QString& mine,
                 const QString& path,
                 const QString& dir,
                 const QString& extra = QString::null,
                 bool isSymlink = FALSE );
    void addSymlink( const QString& mine,
                     QFileInfo* info,
                     const QString& extra = QString::null,
                     bool isSymlink = FALSE);
    void addSymlink( const QString& mine,
                     const QString& path,
                     const QString& name,
                     const QString& extra = QString::null,
                     bool isSymlink = FALSE );
    OFileSelector* view();
    OPixmapProvider* provider();
private:
    OFileSelector* m_view;
    OPixmapProvider* m_prov;
    OListerCmbAccess* m_acc;

    class Private;
    Private *d;
};

class OListerCmbAccess {
    friend class OLister;
public:
    OListerCmbAccess( QComboBox* = 0l);
    ~OListerCmbAccess();

    /**
     * clears the combobox
     */
    void clear();

    /**
     * set's @param add to be the current Item
     * if the item is not present it'll be removed
     */
    void setCurrentItem( const QString& add,  bool FORECE_ADD = TRUE );

    /**
     * inserts the the String at
     * a non predictable position... The position is determined
     * by the QComboBox code
     */
    void insert( const QString& );

    /**
     *
     */
    QString currentText()const;

private:
    class Private;
    Private* d;
    QComboBox* m_cmb;

};

#endif
