/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef PLAY_LIST_WIDGET_H
#define PLAY_LIST_WIDGET_H

#include <qmainwindow.h>
#include <qpe/applnk.h>


class PlayListWidgetPrivate;
class Config;


class PlayListWidget : public QMainWindow {
    Q_OBJECT
public:
    PlayListWidget( QWidget* parent=0, const char* name=0, WFlags fl=0 );
    ~PlayListWidget();

    // retrieve the current playlist entry (media file link)
    const DocLnk *current();
    void useSelectedDocument();

public slots:
    void setDocument( const QString& fileref );
    void addToSelection( const DocLnk& ); // Add a media file to the playlist
    void setActiveWindow(); // need to handle this to show the right view
    void setPlaylist( bool ); // Show/Hide the playlist
    void setView( char );
    void clearList(); 
    void addAllToList(); 
    void addAllMusicToList(); 
    void addAllVideoToList(); 
    void saveList();  // Save the playlist
    void loadList();  // Load a playlist
    bool first();
    bool last();
    bool next();
    bool prev();

private:
    void initializeStates();
    void readConfig( Config& cfg );
    void writeConfig( Config& cfg ) const;
    PlayListWidgetPrivate *d; // Private implementation data
};


#endif // PLAY_LIST_WIDGET_H

