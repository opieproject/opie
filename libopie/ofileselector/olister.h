#ifndef OPIE_FILE_LISTER_H
#define OPIE_FILE_LISTER_H

#include <qfileinfo.h>
#include <qmap.h>
#include <qstring.h>

class OFileSelector;
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

protected:
    bool showFiles()const;
    bool showDirs()const;
    bool compliesMime( const QString& mime );
    void addFile( const QString& mine,
                  QFileInfo*,
                  bool isSymlink = FALSE );
    void addFile( const QString& mine,
                  const QString& path,
                  const QString& file,
                  bool isSymlink = FALSE );
    void addDir( const QString& mine,
                 QFileInfo*,
                 bool isSymlink = FALSE );
    void addDir( const QString& mine,
                 const QString& path,
                 const QString& dir,
                 bool isSymlink = FALSE );
    void addSymlink( const QString& mine,
                     QFileInfo* info,
                     bool isSymlink = FALSE);
    void addSymlink( const QString& mine,
                     const QString& path,
                     const QString& name,
                     bool isSymlink = FALSE );
    OFileSelector* view();
private:
    OFileSelector* m_view;


};

#endif
