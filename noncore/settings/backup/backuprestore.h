#ifndef WINDOW_H
#define WINDOW_H

#include <qmainwindow.h>
#include "backuprestorebase.h"
#include <qmap.h>
#include <qlist.h>

class QListViewItem;

class BackupAndRestore : public BackupAndRestoreBase
{

    Q_OBJECT

public:

    BackupAndRestore( QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
    ~BackupAndRestore();

    static QString appName() { return QString::fromLatin1("backup"); }

private slots:
    void backup();
    void restore();
    void selectItem(QListViewItem *currentItem);
    void sourceDirChanged(int);
    void rescanFolder(QString directory);
    void fileListUpdate();

    void addLocation();
    void removeLocation();
    void saveLocation();

private:
    void scanForApplicationSettings();
    int getBackupFiles(QString &backupFiles, QListViewItem *parent);
    QString getExcludeFile();
    QMap<QString, QString> backupLocations;
    QList<QListViewItem> getAllItems(QListViewItem *item, QList<QListViewItem> &list);
    void refreshBackupLocations();
    void refreshLocations();

    QListViewItem *systemSettings;
    QListViewItem *applicationSettings;
    QListViewItem *documents;

};

#endif

// backuprestore.h

