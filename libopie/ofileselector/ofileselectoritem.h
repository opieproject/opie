#ifndef OPIE_FILE_SELECTOR_ITEM_H
#define OPIE_FILE_SELECTOR_ITEM_H

#include <qlistview.h>

class OFileSelectorItem : public QListViewItem {
public:
    OFileSelectorItem( QListView* view,
                       const QPixmap&,
                       const QString& path,
                       const QString& date,
                       const QString& size,
                       const QString& dir,
                       bool isLocked,
                       bool isDir = false);
    ~OFileSelectorItem();
    bool isLocked() const;
    QString directory()const;
    bool isDir()const;
    QString path()const;
    QString key(int id, bool );

private:
    bool m_locked : 1;
    bool m_dir    : 1;
    QString m_dirStr;

};

#endif
