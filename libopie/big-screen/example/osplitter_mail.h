/*
 * You may use, modify and distribute this code without any limitation
 */

/*
 * Header file for a more complete email client like
 * layout
 */

#ifndef OPIE_SPLITTER_MAIL_EXAMPLE_H
#define OPIE_SPLITTER_MAIL_EXAMPLE_H

#include <qwidget.h>
#include <qlist.h>
#include <qlistview.h>

class Folder;
class QLabel;

class OSplitter;
class ListViews : public QWidget {
    Q_OBJECT
public:
    static QString appName() { return QString::fromLatin1("osplitter-mail"); }
    ListViews( QWidget* parent, const char * name, WFlags fl );
    ~ListViews();

private:
    void initFolders();
    void initFolder( Folder *folder, unsigned int &count );

    QListView *m_messages, *m_overview;
    QLabel *m_message, *m_attach;
    QList<QListView> m_folders; // used in tab mode
    QList<Folder> m_lstFolders;
    bool m_mode : 1; // bitfield
    OSplitter *m_splitter;
    OSplitter *splitti;
    QListView *folder1;
#if 0
//private slots:
//    void slotFolderChanged( QListViewItem* );
//    void slotMessageChanged();
//    void slotSizeChange( bool, const QSize& );
#endif
};

#endif
