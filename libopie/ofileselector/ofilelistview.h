#ifndef OPIE_FILE_LIST_VIEW_H
#define OPIE_FILE_LIST_VIEW_H

#include <qlistview.h>
#include <qpixmap.h>

#include "ofileview.h"

class OFileListView : public QListView, public OFileView {
    Q_OBJECT
public:
    OFileListView( QWidget* parent, OFileSelector* );
    ~OFileListView();

    void clear();
    void addFile( const QPixmap&,
                  const QString& mine,
                  QFileInfo* info,
                  const QString& extra = QString::null,
                  bool isSymlink = FALSE );

    void addFile( const QPixmap&,
                  const QString& mime,
                  const QString& dir,
                  const QString& file,
                  const QString& extra = QString::null,
                  bool = false );

    void addDir( const QPixmap&,
                 const QString& mime,
                 const QString& extra = QString::null,
                 QFileInfo* info, bool = FALSE );

    void addDir( const QPixmap&,
                 const QString& mime, const QString& dir,
                 const QString& file,
                 const QString& extra = QString::null,
                 bool = FALSE );

    void addSymlink( const QPixmap&,
                     const QString& mime,
                     QFileInfo* info,
                     const QString& extra = QString::null,
                     bool = FALSE );
    void addSymlink( const QPixmap&,
                     const QString& mine, const QString& path,
                     const QString& file,
                     const QString& extra,
                     bool isSymlink = FALSE );
    void cd( const QString& path );
    QWidget* widget();
    void sort();

    QString selectedName()const ;
    QStringList selectedNames()const;

    QString selectedPath()const;
    QStringList selectedPaths()const;
    int fileCount();
private slots:
    void slotSelectionChanged();
    void slotCurrentChanged(QListViewItem* );
    void slotClicked( int, QListViewItem*, const QPoint&, int );
    void slotRightButton(int, QListViewItem*, const QPoint&, int );
};

#endif
