
#include "backuprestore.h"
//#include "output.h"
#include "errordialog.h"

#include <qapplication.h>
#include <qmultilineedit.h>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlist.h>
#include <stdlib.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qtextview.h>
#include <opie2/ostorage.h>

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define HEADER_NAME 0
#define HEADER_BACKUP 1
#define BACKUP_LOCATION 2

#define EXTENSION ".bck"

const QString tempFileName = "/tmp/backup.err";


BackupAndRestore::BackupAndRestore( QWidget* parent, const char* name,  WFlags fl)
        : BackupAndRestoreBase(parent, name,  fl)
{
    this->showMaximized();
    backupList->header()->hide();
    restoreList->header()->hide();
    connect(backupButton, SIGNAL(clicked()),
            this, SLOT(backup()));
    connect(restoreButton, SIGNAL(clicked()),
            this, SLOT(restore()));
    connect(backupList, SIGNAL(clicked( QListViewItem * )),
            this, SLOT(selectItem(QListViewItem*)));
    connect(restoreSource, SIGNAL(activated( int  )),
            this, SLOT(sourceDirChanged(int)));
    connect(updateList, SIGNAL(clicked()),
            this, SLOT( fileListUpdate()));

    //add directorys for backing up
    applicationSettings = new QListViewItem(backupList, "Application Settings", "", "Settings/");
    selectItem(applicationSettings);
    applicationSettings = new QListViewItem(backupList, "Application Data", "", "Applications/");
    selectItem(applicationSettings);
    documents= new QListViewItem(backupList, "Documents", "", "Documents/");
    selectItem(documents);

    scanForApplicationSettings();

    OStorageInfo storage;

    backupLocations.insert( "Documents", QDir::homeDirPath() + "/Documents" );
    if ( storage.hasCf() )
    {
        backupLocations.insert( "CF", storage.cfPath() );
        qDebug( "Cf Path: " + storage.cfPath() );
    }
    if ( storage.hasSd() )
    {
        backupLocations.insert( "SD", storage.sdPath() );
        qDebug( " Sd Path: " + storage.sdPath() );
    }
    if ( storage.hasMmc() )
    {
        backupLocations.insert( "MMC", storage.mmcPath() );
        qDebug( "Mmc Path: " + storage.mmcPath() );
    }

    Config config("BackupAndRestore");
    //read last locations
    config.setGroup("LastLocation");
    QString lastStoreLocation   = config.readEntry( "LastStoreLocation", "" );
    QString lastRestoreLocation = config.readEntry( "LastRestoreLocation", "" );
    int locationIndex    = 0;

    QMap<QString, QString>::Iterator it;
    for( it = backupLocations.begin(); it != backupLocations.end(); ++it )
    {
        storeToLocation->insertItem(it.key());
        restoreSource->insertItem(it.key());

        //check for last locations
        if ( it.key() == lastStoreLocation )
            storeToLocation->setCurrentItem( locationIndex );
        if ( it.key() == lastRestoreLocation )
            restoreSource->setCurrentItem( locationIndex );
        locationIndex++;
    }

    // Read the list of items to ignore.
    QList<QString> dontBackupList;
    dontBackupList.setAutoDelete(true);
    config.setGroup("DontBackup");
    int total = config.readNumEntry("Total", 0);
    for(int i = 0; i < total; i++)
    {
        dontBackupList.append(new QString(config.readEntry(QString("%1").arg(i), "")));
    }

    QList<QListViewItem> list;
    getAllItems(backupList->firstChild(), list);

    for(uint i = 0; i < list.count(); i++)
    {
        QString text = list.at(i)->text(HEADER_NAME);
        for(uint i2 = 0;  i2 < dontBackupList.count(); i2++)
        {
            if(*dontBackupList.at(i2) == text)
            {
                selectItem(list.at(i));
                break;
            }
        }
    }
}

BackupAndRestore::~BackupAndRestore()
{
    QList<QListViewItem> list;
    getAllItems(backupList->firstChild(), list);

    Config config("BackupAndRestore");
    config.setGroup("DontBackup");
    config.clearGroup();

    int count = 0;
    for(uint i = 0; i < list.count(); i++)
    {
        if(list.at(i)->text(HEADER_BACKUP) == "")
        {
            config.writeEntry(QString("%1").arg(count), list.at(i)->text(HEADER_NAME));
            count++;
        }
    }
    config.writeEntry("Total", count);

    // Remove Temp File
    if ( QFile::exists( tempFileName ) )
        QFile::remove( tempFileName );
}

QList<QListViewItem> BackupAndRestore::getAllItems(QListViewItem *item, QList<QListViewItem> &list)
{
    while(item)
    {
        if(item->childCount() > 0)
            getAllItems(item->firstChild(), list);
        list.append(item);
        item = item->nextSibling();
    }
    return list;
}

/**
 * Selects and unselects the item by setting the HEADER_BACKUP to B or !.
 * and changing the icon to match
 * @param currentItem the item to swich the selection choice.
 */
void BackupAndRestore::selectItem(QListViewItem *currentItem)
{
    if(!currentItem)
        return;

    if(currentItem->text(HEADER_BACKUP) == "B")
    {
        currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/null"));
        currentItem->setText(HEADER_BACKUP, "");
    }
    else
    {
        currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/check"));
        currentItem->setText(HEADER_BACKUP, "B");
    }
}

void BackupAndRestore::scanForApplicationSettings()
{
    QDir d( QDir::homeDirPath() + "/" + QString( applicationSettings->text(BACKUP_LOCATION) ) );
    d.setFilter( QDir::Dirs | QDir::Files | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
        //qDebug((d.path()+"/"+fi->fileName()).latin1());
        if ( ( fi->fileName() != "." ) && ( fi->fileName() != ".." ) )
        {
            QListViewItem *newItem = new QListViewItem(applicationSettings, fi->fileName());
            selectItem(newItem);
        }
        ++it;
    }
}

/**
 * The "Backup" button has been pressed.  Get a list of all of the files that
 * should be backed up.  If there are no files, emit and error and exit.
 * Determine the file name to store the backup in.  Backup the file(s) using
 * tar and gzip  --best.  Report failure or success
 */
void BackupAndRestore::backup()
{
    QString backupFiles;
    if(getBackupFiles(backupFiles, NULL) == 0)
    {
        QMessageBox::critical(this, "Message",
                              "No items selected.",QString("Ok") );
        return;
    }

    setCaption(tr("Backup and Restore... working..."));
    QString outputFile = backupLocations[storeToLocation->currentText()];

    QDateTime datetime = QDateTime::currentDateTime();
    QString dateString = QString::number( datetime.date().year() ) + QString::number( datetime.date().month() ).rightJustify(2, '0') +
                         QString::number( datetime.date().day() ).rightJustify(2, '0');

    outputFile += "/" + dateString;

    QString t = outputFile;
    int c = 1;
    while(QFile::exists(outputFile + EXTENSION))
    {
        outputFile = t + QString("%1").arg(c);
        c++;
    }

    // We execute tar and compressing its output with gzip..
    // The error output will be written into a temp-file which could be provided
    // for debugging..
    qDebug( "Storing file: %s", outputFile.latin1() );
    outputFile += EXTENSION;

    QString commandLine = QString( "(tar -C %1 -c %2 | gzip > %3 ) 2> %4" ).arg( QDir::homeDirPath() )
                                                                           .arg( backupFiles )
                                                                           .arg( outputFile.latin1() )
                                                                           .arg( tempFileName.latin1() );

    qDebug( commandLine );

    int r = system( commandLine );

    if(r != 0)
    {
        perror("Error: ");
        QString errorMsg= tr( "Error from System:\n" ) + (QString)strerror( errno );

        switch( QMessageBox::critical(this, tr( "Message" ), tr( "Backup Failed!" ) + "\n"
                                      + errorMsg, QString( tr( "Ok" ) ), QString( tr( "Details" ) ) ) )
        {

        case 1:
            qWarning("Details pressed !");
            ErrorDialog* pErrDialog = new ErrorDialog( this, NULL, true );
            QFile errorFile( tempFileName );
            if ( errorFile.open(IO_ReadOnly) )
            {
                QTextStream t( &errorFile );
                QString s;
                while ( !t.eof() )
                {        // until end of file...
                    s += t.readLine();       // line of text excluding '\n'
                }
                errorFile.close();

                pErrDialog->m_textarea->setText( s );
            }
            else
            {
                pErrDialog->m_textarea->setText( "Unable to open File: /tmp/backup.er" );
            }
            pErrDialog->showMaximized();
            pErrDialog->exec();
            delete pErrDialog;
            break;
        }
        setCaption(tr("Backup and Restore.. Failed !!"));
        return;
    }
    else
    {
        QMessageBox::information(this, tr( "Message" ), tr( "Backup Successful." ), QString(tr( "Ok" ) ) );

    }

    //write store-location
    Config config( "BackupAndRestore" );
    config.setGroup( "LastLocation" );
    config.writeEntry( "LastStoreLocation", storeToLocation->currentText() );

    setCaption(tr("Backup and Restore"));
}

/***
 * Get a list of all of the files to backup.
 */
int BackupAndRestore::getBackupFiles(QString &backupFiles, QListViewItem *parent)
{
    QListViewItem * currentItem;
    QString currentHome;
    if(!parent)
        currentItem = backupList->firstChild();
    else
    {
        currentItem = parent->firstChild();
        currentHome = parent->text(BACKUP_LOCATION);
    }

    uint count = 0;
    while( currentItem != 0 )
    {
        if(currentItem->text(HEADER_BACKUP) == "B" )
        {
            if(currentItem->childCount() == 0 )
            {
                if(parent == NULL)
                    backupFiles += currentItem->text(BACKUP_LOCATION);
                else
                    backupFiles += currentHome + currentItem->text(HEADER_NAME);
                backupFiles += " ";
                count++;
            }
            else
            {
                count += getBackupFiles(backupFiles, currentItem);
            }
        }
        currentItem = currentItem->nextSibling();
    }
    return count;
}

void BackupAndRestore::sourceDirChanged(int selection)
{
    restoreList->clear();
    rescanFolder(backupLocations[restoreSource->text(selection)]);
}

void BackupAndRestore::fileListUpdate()
{
    qWarning("void BackupAndRestore::fileListUpdate()");
    restoreList->clear();
    rescanFolder( backupLocations[restoreSource->currentText()] );
}

/**
 * Scans directory for any backup files.  Will recursivly go down,
 * but will not follow symlinks.
 * @param directory - the directory to look in.
 */
void BackupAndRestore::rescanFolder(QString directory)
{
    //qDebug(QString("rescanFolder: ") + directory.latin1());
    QDir d(directory);
    if(!d.exists())
        return;

    d.setFilter( QDir::Files | QDir::Hidden | QDir::Dirs);
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *file;
    while ( (file=it.current()) )
    {  // for each file...
        // If it is a dir and not .. or . then add it as a tab and go down.
        if(file->isDir())
        {
            if(file->fileName() != ".." && file->fileName() != ".")
            {
                rescanFolder(directory + "/" + file->fileName());
            }
        }
        else
        {
            // If it is a backup file add to list.
            if(file->fileName().contains(EXTENSION))
                (void)new QListViewItem(restoreList, file->fileName());
        }
        ++it;
    }
}

/**
 * Restore a backup file.
 * Report errors or success
 */
void BackupAndRestore::restore()
{
    QListViewItem *restoreItem = restoreList->currentItem();
    if(!restoreItem)
    {
        QMessageBox::critical(this, tr( "Message" ),
                              tr( "Please select something to restore." ),QString( tr( "Ok") ) );
        return;
    }
    setCaption(tr("Backup and Restore... working..."));

    QString restoreFile = backupLocations[restoreSource->currentText()];

    restoreFile += "/" + restoreItem->text(0);

    qDebug( restoreFile );

    QString commandLine = QString( "tar -C %1 -zxf %2 2> %3" ).arg( QDir::homeDirPath() )
                                                              .arg( restoreFile.latin1() )
                                                              .arg( tempFileName.latin1() );

    qDebug( commandLine );

    int r = system( commandLine );

    if(r != 0)
    {
        QString errorMsg= tr( "Error from System:\n" ) + (QString)strerror( errno );
        switch( QMessageBox::critical(this, tr( "Message" ), tr( "Restore Failed." ) + "\n"
                                      + errorMsg, QString( tr( "Ok") ), QString( tr( "Details" ) ) ) )
        {
        case 1:
            qWarning("Details pressed !");
            ErrorDialog* pErrDialog = new ErrorDialog( this, NULL, true );
            QFile errorFile( tempFileName );
            if ( errorFile.open(IO_ReadOnly) )
            {
                QTextStream t( &errorFile );
                QString s;
                while ( !t.eof() )
                {        // until end of file...
                    s += t.readLine();       // line of text excluding '\n'
                }
                errorFile.close();

                pErrDialog->m_textarea->setText( s );
            }
            else
            {
                pErrDialog->m_textarea->setText( tr( "Unable to open File: %1" ).arg( "/tmp/backup.er" ) );
            }
            pErrDialog->showMaximized();
            pErrDialog->exec();
            delete pErrDialog;

            setCaption(tr("Backup and Restore.. Failed !!"));
            return;

            break;

        }
    }
    else
    {
        QMessageBox::information(this, tr( "Message" ), tr( "Restore Successful." ), QString( tr( "Ok") ) );
    }

    //write restore-location
    Config config( "BackupAndRestore" );
    config.setGroup( "LastLocation" );
    config.writeEntry( "LastRestoreLocation", restoreSource->currentText() );

    setCaption(tr("Backup and Restore"));
}

// backuprestore.cpp

