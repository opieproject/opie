
#ifndef PLAY_LIST_WIDGET_H
#define PLAY_LIST_WIDGET_H

#include <qmainwindow.h>
#include <qpe/applnk.h>
#include <qtabwidget.h>
#include <qpe/fileselector.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>

/* #include <qtimer.h> */


class PlayListWidgetPrivate;
class Config;
class QListViewItem;
class QListView;
class QPoint;
class QAction;
class QLabel;

class PlayListWidget : public QMainWindow {
    Q_OBJECT
public:
    PlayListWidget( QWidget* parent=0, const char* name=0, WFlags fl=0 );
    ~PlayListWidget();
    QTabWidget * tabWidget;
//    MenuItem *fullScreenButton, *scaleButton;
//    QAction *fullScreenButton, *scaleButton;
    DocLnkSet files;
    DocLnkSet vFiles;
    QListView *audioView, *videoView, *playlistView;
    QLabel *libString;
    QPopupMenu *pmView ;
    bool fromSetDocument;
    bool insanityBool;
    QString setDocFileRef;
    // retrieve the current playlist entry (media file link)
    const DocLnk *current();
    void useSelectedDocument();
/*     QTimer    * menuTimer; */
    FileSelector*  playLists;
    QPushButton *tbDeletePlaylist;
    int fd, selected;
public slots:
    bool first();
    bool last();
    bool next();
    bool prev();
/*     void setFullScreen(); */
/*     void setScaled(); */
protected:
/*     void contentsMousePressEvent( QMouseEvent * e ); */
/*     void contentsMouseReleaseEvent( QMouseEvent * e ); */
void keyReleaseEvent( QKeyEvent *e);
void keyPressEvent( QKeyEvent *e);
private:
    int defaultSkinIndex; 
    QPopupMenu *skinsMenu; 
    bool audioScan, videoScan;
    void doBlank();
    void doUnblank();
    void readm3u(const QString &);
    void readPls(const QString &);

    void initializeStates();
    void readConfig( Config& cfg );
    void writeConfig( Config& cfg ) const;
    PlayListWidgetPrivate *d; // Private implementation data
    void populateAudioView();
    void populateVideoView();
private slots:
    void populateSkinsMenu();
    void skinsMenuActivated(int); 
    void pmViewActivated(int);
    void writem3u();
    void scanForAudio();
    void scanForVideo();
    void openFile();
    void setDocument( const QString& fileref );
    void addToSelection( const DocLnk& ); // Add a media file to the playlist
    void addToSelection( QListViewItem* ); // Add a media file to the playlist
    void setActiveWindow(); // need to handle this to show the right view
    void setPlaylist( bool ); // Show/Hide the playlist
    void setView( char );
    void clearList();
    void addAllToList();
    void addAllMusicToList();
    void addAllVideoToList();
    void saveList();  // Save the playlist
    void loadList( const DocLnk &);  // Load a playlist
    void playIt( QListViewItem *);

    void btnPlay(bool);
    void deletePlaylist();
    void addSelected();
    void removeSelected();
    void tabChanged(QWidget*);
    void viewPressed( int, QListViewItem *, const QPoint&, int);
    void playlistViewPressed( int, QListViewItem *, const QPoint&, int);
    void playSelected();
    void listDelete();

protected slots:
/*     void cancelMenuTimer(); */
/*     void showFileMenu(); */


};


#endif // PLAY_LIST_WIDGET_H

