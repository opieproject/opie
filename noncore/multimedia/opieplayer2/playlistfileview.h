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

    DocLnkSet &files() { return m_files; }

public slots:
    void scanFiles();

private:
    QString m_mimeTypePattern;
    DocLnkSet m_files;
};

#endif // PLAYLISTFILEVIEW_H
/* vim: et sw=4 ts=4
 */
