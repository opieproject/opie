#ifndef PLAYLISTFILEVIEW_H
#define PLAYLISTFILEVIEW_H

#include "playlistview.h"

#include <qpe/applnk.h>

class PlayListFileView : public PlayListView
{
    Q_OBJECT
public:
    PlayListFileView( const QString &mimeTypePattern, const QString &itemPixmapName, QWidget *parent, const char *name = 0 );
    virtual ~PlayListFileView();

public slots:
    void scanFiles();
    void populateView();

private:
    QString m_mimeTypePattern;
    QString m_itemPixmapName;
    DocLnkSet m_files;
    bool m_scannedFiles;
    bool m_viewPopulated;
};

#endif // PLAYLISTFILEVIEW_H
/* vim: et sw=4 ts=4
 */
