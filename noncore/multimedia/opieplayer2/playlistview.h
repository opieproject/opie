#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <qlistview.h>

class PlayListView : public QListView
{
    Q_OBJECT
public:
    PlayListView( QWidget *parent, const char *name );
    virtual ~PlayListView();
};

#endif // PLAYLISTVIEW_H
/* vim: et sw=4 ts=4
 */
