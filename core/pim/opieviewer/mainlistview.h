
#ifndef MAIN_LIST_VIEW_H
#define MAIN_LIST_VIEW_H

#include <qlistview.h>
#include <qlist.h>

#include "mainlistviewitem.h"

namespace Viewer{
    //class MainListViewItem;

    class MainListView : public QListView {
        Q_OBJECT
    public:
        MainListView( QWidget* parent, const char* name = 0 );
        ~MainListView();
        void insertCategory( const QString& name, int id );
        void insertEntry( int id,
                          const QString& category,
                          int categoryId,
                          const QString& appName,
                          const QString& text,
                          const QString& richText,
                          const QPixmap& pixmap );
        void insertRelation( const QString& appSrc, int idSrc,
                             const QString& appRel, int relateWith );
        void clear();
        int currentId()const { return -1;};
        QString currentApp()const { return QString::null; };

    signals:
        void richText( int id,  const QString& );
        void selected( const QString& app, const QString& richText );
        void edit(const QString& app, int id );
        void show(const QString& app );
        void newEntry( const QString& app );
    private:
        QList<MainListViewItem> m_items;
        MainListViewItem* find(const QString& app, int id );

    private slots:
        void slotSelectionChanged( );
        void slotCurrentChanged( QListViewItem* );
        void slotClicked( int, QListViewItem*,const QPoint&,int );
        void slotRightButton(int, QListViewItem*, const QPoint&,  int );


    };
}

#endif
