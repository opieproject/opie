#ifndef OPIE_PIXMPAP_PROVIDER
#define OPIE_PIXMPAP_PROVIDER

#include <qpixmap.h>

#include "ofileselector.h"

/**
 * This is a basic interface for a Pixmap provider
 * it provides pixmaps for a URL
 * So the OLocalLister provides mimetype icons
 */
class QFileInfo;
class OPixmapProvider {
public:
    /* you can or these
     * but File | Dir would be insane ;)
     */
    enum Type { File = 0,  Dir = 1,  Symlink = 2 };
    OPixmapProvider( OFileSelector* sel );
    virtual ~OPixmapProvider();

    virtual QPixmap pixmap( int t, const QString& mime, QFileInfo*,
                            bool isLocked = FALSE );
    virtual QPixmap pixmap( int t, const QString& mime,
                            const QString& dir, const QString& file,
                            bool isLocked = FALSE );

protected:
    OFileSelector* selector();
private:
    QPixmap file( QFileInfo*,
                  bool sym, bool is );
    QPixmap dir ( QFileInfo*,
                  bool sym, bool is );
    QPixmap sym ( QFileInfo*,
                  bool is );
    OFileSelector* m_sel;
};

#endif
