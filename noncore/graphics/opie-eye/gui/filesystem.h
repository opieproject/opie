/*
 * GPLv2 zecke@handhelds.org
 * No WArranty...
 */

#ifndef PHUNK_FILE_SYSTEM_H
#define PHUNK_FILE_SYSTEM_H

#include <qtoolbutton.h>
#include <qmap.h>

class QPopupMenu;
class StorageInfo;
class PFileSystem : public QToolButton {
    Q_OBJECT
public:
    PFileSystem( QToolBar* );
    ~PFileSystem();

signals:
    void changeDir( const QString& );

private slots:
    void slotSelectDir( int );
    void changed();

private:
    QPopupMenu* m_pop;
    StorageInfo *m_storage;
    QMap<QString, QString> m_dev;
};


#endif
