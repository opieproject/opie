#ifndef PLAYLISTFILEVIEW_H
#define PLAYLISTFILEVIEW_H

#include "playlistview.h"

class PlayListFileView : public PlayListView
{
    Q_OBJECT
public:
    PlayListFileView( QWidget *parent, const char *name = 0 );
    virtual ~PlayListFileView();
};

#endif // PLAYLISTFILEVIEW_H
/* vim: et sw=4 ts=4
 */
