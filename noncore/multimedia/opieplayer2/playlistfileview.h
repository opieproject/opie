#ifndef PLAYLISTFILEVIEW_H
#define PLAYLISTFILEVIEW_H

#include "playlistview.h"

#include <qpe/applnk.h>

class PlayListFileView : public PlayListView
{
    Q_OBJECT
public:
    PlayListFileView( const QString &mimeTypePattern, QWidget *parent, const char *name = 0 );
    virtual ~PlayListFileView();

    // both temporarily accessible that way until the caller code has
    // been migrated into this class
    DocLnkSet &files() { return m_files; }
    bool &scannedFiles() { return m_scannedFiles; }

public slots:
    void scanFiles();

private:
    QString m_mimeTypePattern;
    DocLnkSet m_files;
    bool m_scannedFiles;
};

#endif // PLAYLISTFILEVIEW_H
/* vim: et sw=4 ts=4
 */
