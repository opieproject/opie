#ifndef MAIN_LIST_VIEW_ITEM_H
#define MAIN_LIST_VIEW_ITEM_H

#include <qpixmap.h>
#include <qlistview.h>

namespace Viewer {
    class MainListViewItem : public QListViewItem {
    public:
        MainListViewItem(QListView* parent,  const QString& category,  int id);

        MainListViewItem(QListViewItem* parent,
                      const QString& appName,
                      const QString& text,
                      const QString& richText,
                      const QPixmap& pixmap,
                      int id );
        ~MainListViewItem();
        bool isCategory()const;
        QString category()const;
        QString appName()const;
        QString text()const;
        QString richText()const;
        QPixmap pixmap()const;
        /**
         * Either category or data id
         */
        int id()const;

    private:
        bool m_category:1;
        QString m_catName;
        QString m_appName;
        QString m_text;
        QString m_richText;
        QPixmap m_pixmap;
        int m_id;
    };
}

#endif
