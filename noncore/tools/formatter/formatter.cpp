/****************************************************************************
** formatter.cpp
**
** Copyright: Thu Apr 11 11:01:13 2002
**      by:  L. J. Potter
**
****************************************************************************/

#include "formatter.h"
#include "inputDialog.h"
#include "output.h"

#include <qpe/qpemenubar.h>
#include <qpe/qpetoolbar.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/mimetype.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/storage.h>

#include <qmultilineedit.h>
#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qfile.h>

#include <qtstream.h>

#include <qcombobox.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <string.h>
#include <errno.h>

#define BLANK ' '
#define DELIMITER '#'

/* 
Blah blah blah blah */
FormatterApp::FormatterApp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QMainWindow( parent, name, fl )
//    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "FormatterApp" );
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    setCaption( tr( "Formatter" ) );
    FormatterAppLayout = new QGridLayout( this ); 
    FormatterAppLayout->setSpacing( 2);
    FormatterAppLayout->setMargin( 2 );

    TabWidget = new QTabWidget( this, "TabWidget" );

    tab = new QWidget( TabWidget, "tab" );
    tabLayout = new QGridLayout( tab ); 
    tabLayout->setSpacing( 6);
    tabLayout->setMargin( 11);

    storageComboBox = new QComboBox( FALSE, tab, "storageComboBox" );

    tabLayout->addMultiCellWidget( storageComboBox, 0, 0, 0, 1 );

    TextLabel4 = new QLabel( tab, "TextLabel4" );
    TextLabel4->setText( tr( "Storage Type" ) );

    tabLayout->addMultiCellWidget( TextLabel4, 1, 1, 0, 1 );

    TextLabel2 = new QLabel( tab, "TextLabel2" );
    TextLabel2->setText( tr( "File Systems" ) );

    tabLayout->addMultiCellWidget( TextLabel2, 4, 4, 0, 1 );

    fileSystemsCombo = new QComboBox( FALSE, tab, "fileSystemsCombo" );

    tabLayout->addMultiCellWidget( fileSystemsCombo, 3, 3, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer, 2, 1 );

    formatPushButton = new QPushButton( tab, "formatPushButton" );
    formatPushButton->setText( tr( "Format" ) );

    tabLayout->addMultiCellWidget( formatPushButton, 6, 6, 0, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout->addItem( spacer_2, 5, 0 );
    TabWidget->insertTab( tab, tr( "Main" ) );

    tab_2 = new QWidget( TabWidget, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2 ); 
    tabLayout_2->setSpacing( 6);
    tabLayout_2->setMargin( 11);

    mountPointLineEdit = new QLineEdit( tab_2, "mountPointLineEdit" );

    tabLayout_2->addMultiCellWidget( mountPointLineEdit, 0, 0, 0, 1 );

    deviceComboBox = new QComboBox( FALSE, tab_2, "deviceComboBox" );

    tabLayout_2->addMultiCellWidget( deviceComboBox, 3, 3, 0, 1 );

    TextLabel5 = new QLabel( tab_2, "TextLabel5" );
    TextLabel5->setText( tr( "CAUTION:\n"
                             "Changing parameters on this \n"
                             "page may cause your system \n"
                             "to stop functioning properly!!" ) );//idiot message

    tabLayout_2->addMultiCellWidget( TextLabel5, 6, 6, 0, 1 );

    editPushButton = new QPushButton( tab_2, "editPushButton" );
    editPushButton->setText( tr( "Edit fstab" ) );

    tabLayout_2->addMultiCellWidget( editPushButton, 7, 7, 0, 0 );

    fsckButton = new QPushButton( tab_2, "fsckPushButton" );
    fsckButton->setText( tr( "Check Disk" ) );

    tabLayout_2->addMultiCellWidget( fsckButton, 7, 7, 1, 1 );

    TextLabel3 = new QLabel( tab_2, "TextLabel3" );
    TextLabel3->setText( tr( "Device" ) );

    tabLayout_2->addMultiCellWidget( TextLabel3, 4, 4, 0, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_3, 5, 0 );

    TextLabel1 = new QLabel( tab_2, "TextLabel1" );
    TextLabel1->setText( tr( "Mount Point" ) );

    tabLayout_2->addMultiCellWidget( TextLabel1, 1, 1, 0, 1 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_4, 2, 1 );
    TabWidget->insertTab( tab_2, tr( "Advanced" ) );

    FormatterAppLayout->addWidget( TabWidget, 0, 0 );

    connect( formatPushButton ,SIGNAL(released()),this,SLOT( doFormat()) );
    connect( editPushButton ,SIGNAL(released()),this,SLOT( editFstab()) );
    connect( fsckButton ,SIGNAL(released()),this,SLOT( doFsck()) );

    connect( fileSystemsCombo,SIGNAL(activated(int)),this,SLOT( fsComboSelected(int ) ));
    connect( storageComboBox,SIGNAL(activated(int)),this,SLOT( storageComboSelected(int ) ));
    connect( deviceComboBox,SIGNAL(activated(int)),this,SLOT( deviceComboSelected(int ) ));

    
    fillCombos();
}

/*  
 *  Destroys the object and frees any allocated resources
 frickin no duh?!?
 */
FormatterApp::~FormatterApp()
{

}

void FormatterApp::doFormat() {
    int err=0;
    Output *outDlg;
    QString umountS, remountS;
    QString text =  storageComboBox->currentText();
    QString currentText = storageComboBox->currentText();
    QString cmd;
    QString diskDevice = currentText.right( currentText.length() - currentText.find(" -> ",0,TRUE) - 4);
    QString diskName =  currentText.left(currentText.find(" -> ",0,TRUE));
    QString fs = fileSystemsCombo->currentText();

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX) // lets test on something cheap
#else
    currentText  = diskDevice  = "/dev/fd0";
    umountS = "umount -v /floppy 2>&1";
    remountS = "mount -v /floppy 2>&1";
#endif

    if( currentText.find("CF",0,TRUE) != -1) {
        umountS = "umount ";
        remountS = "mount ";

//         umountS = "/sbin/cardctl eject";
//         remountS = "/sbin/cardctl insert";
    }
    if( currentText.find("SD",0,TRUE) != -1) {
        umountS = "umount ";
        remountS = "mount ";
//         umountS = "/etc/sdcontrol compeject";
//         remountS = "/etc/sdcontrol insert";
    }

    switch ( QMessageBox::warning(this,tr("Format?!?"),tr("Really format\n") +diskName+" "+ currentText +
                                  tr("\nwith ") + fs + tr(" filesystem?!?\nYou will loose all data!!"),tr("Yes"),tr("No"),0,1,1) ) {
      case 0: {
          if(fs == "vfat")
              cmd = "mkdosfs -v " + diskDevice+" 2>&1";
          else if(fs == "ext2")
              cmd = "mke2fs -v " + diskDevice+" 2>&1";
          else {
              QMessageBox::warning(this, tr("Formatter"),tr("Could not format.\nUnknown type"), tr("Ok"));
              break;
          }
//          cmd = "ls -l";
          outDlg = new Output(this, tr("Formatter Output"),FALSE);
          outDlg->showMaximized();
          outDlg->show();
          qApp->processEvents();
          FILE *fp;
          char line[130];
          outDlg->OutputEdit->append( tr("Trying to umount.") + currentText );
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);

          sleep(1);
          qDebug("Command is "+umountS);
          fp = popen(  (const char *) umountS, "r");
          qDebug("%d", fp);
          if ( !fp ) {
              qDebug("Could not execute '" + umountS + "'! err=%d\n" +(QString)strerror(errno), err); 
              QMessageBox::warning( this, tr("Formatter"), tr("umount failed!"), tr("&OK") );
              pclose(fp);             
              return;
          } else {
//               outDlg->OutputEdit->append( currentText + tr("\nhas been successfully umounted."));
//             outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              while ( fgets( line, sizeof line, fp)) {
                  if( ((QString)line).find("busy",0,TRUE) != -1) {
                      qDebug("Could not find '" + umountS); 
                      QMessageBox::warning( this, tr("Formatter"), tr("Could not umount.\nDevice is busy!"), tr("&OK") );
                      pclose(fp);             
                      return;
                  } else {
                      QString lineStr = line;
                      lineStr=lineStr.left(lineStr.length()-1);
                      outDlg->OutputEdit->append(lineStr);
                      outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
                  }
              }
          }
          pclose(fp);             

          qDebug("Command would be: "+cmd);
          outDlg->OutputEdit->append( tr("Trying to format.") );
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);

          fp = popen(  (const char *) cmd, "r"); 
          while ( fgets( line, sizeof line, fp)) {
              if( ((QString)line).find("No such device",0,TRUE) != -1) {
                  qDebug("No such device '" + umountS); 
                  QMessageBox::warning( this, tr("Formatter"), tr("No such device!"), tr("&OK") );
                  pclose(fp);             
//               outDlg->OutputEdit->append("No such device");
//             outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
                  return;
              } else {
                  QString lineStr = line;
                  lineStr=lineStr.left(lineStr.length()-1);
                  outDlg->OutputEdit->append(lineStr);
                  outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              }
          }
          outDlg->OutputEdit->append( currentText + tr("\nhas been successfully formatted."));
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
          pclose(fp);             

          outDlg->OutputEdit->append( tr("Trying to mount.") + currentText );
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
          fp = popen(  (const char *) remountS, "r"); 
          if ( !fp) {
              qDebug("Could not execute '" + remountS + "'! err=%d\n" +(QString)strerror(errno), err); 
              QMessageBox::warning( this, tr("Formatter"), tr("Card mount failed!"), tr("&OK") );

          } else {
              outDlg->OutputEdit->append( currentText + tr("\nhas been successfully mounted."));
              while ( fgets( line, sizeof line, fp)) {
                  QString lineStr = line;
                  lineStr=lineStr.left(lineStr.length()-1);
                  outDlg->OutputEdit->append(lineStr);
                  outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              }
          }
          pclose(fp);
          sleep(1);
          
          outDlg->OutputEdit->append(tr("You can now close the output window."));
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
//          outDlg->close();
//            if(outDlg)
//            delete outDlg;
      }
          break;
    };
}

bool FormatterApp::doFdisk() {
    return FALSE;
    
}

void FormatterApp::fillCombos() {

    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString storage;
    for( ; it.current(); ++it ){
        const QString name = (*it)->name();
        const QString path = (*it)->path();
        const QString disk = (*it)->disk();
        const QString options = (*it)->options();
        if( name.find( tr("Internal"),0,TRUE) == -1) {
            storageComboBox->insertItem(name +" -> "+disk);
        }
//        deviceComboBox->insertItem(disk);
    }
     parsetab("/etc/mtab");
//    parsetab("/etc/fstab");
    fileSystemsCombo->insertStringList( fsList,-1);
    deviceComboBox->insertStringList( deviceList,-1);
    storageComboSelected(0);
    deviceComboSelected(0); 
}


void FormatterApp::fsComboSelected(int index) {

}
    
void FormatterApp::storageComboSelected(int index ) {

    QString currentText = storageComboBox->text(index);
    QString nameS = currentText.left( currentText.find("->",0,TRUE));

    TextLabel4->setText( tr( "Storage Type : ") + nameS );
    currentText = currentText.right( currentText.length() - currentText.find(" -> ",0,TRUE) - 4);

    QString fsType = getFileSystemType((const QString &) currentText);
//    qDebug(fsType);
    for(int i = 0; i < fileSystemsCombo->count(); i++) {
        if( fsType == fileSystemsCombo->text(i))
             fileSystemsCombo->setCurrentItem(i);
    }
//    deviceComboSelected(index);     
}

void FormatterApp::deviceComboSelected(int index) {

    StorageInfo storageInfo;
    QString totalS, usedS, avS, diskS, nameS, fsType, selectedText;

    selectedText = deviceComboBox->text(index);

    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString storage;
    for( ; it.current(); ++it ){
        const QString name = (*it)->name();
        const QString path = (*it)->path();
        const QString disk = (*it)->disk();
//              const QString options = (*it)->options();
        if( selectedText == disk) {
            diskS = disk; nameS= name;
            mountPointLineEdit->setText(path);
            long mult = (*it)->blockSize() / 1024;
            long div = 1024 / (*it)->blockSize();
            if ( !mult ) mult = 1;
            if ( !div ) div = 1;
            long total = (*it)->totalBlocks() * mult / div;
            long totalMb = total/1024;
            long avail = (*it)->availBlocks() * mult / div;
            long availMb = avail/1024;
            long used = total - avail;
            long usedMb = used/1024;
            totalS.sprintf(tr("Total: %1 kB ( %d mB)\n").arg( total ), totalMb );
            usedS.sprintf(tr("Used: %1 kB ( %d mB)\n").arg(used) ,usedMb);
            avS.sprintf( tr("Available: %1 kB ( %d mB)").arg(avail), availMb );
        }
    }
    fsType = getFileSystemType((const QString &)selectedText);

    TextLabel5->setText("Type: "+ nameS+" Formatted with "+ fsType + " \n" + totalS + usedS + avS);
//     storageComboSelected(0);
}

void FormatterApp::cleanUp() {

}

    
void FormatterApp::editFstab() {
    QCopEnvelope e("QPE/Application/textedit","setDocument(QString)");
    e << (const QString &)"/etc/fstab";
}

void FormatterApp::parsetab(const QString &fileName) {

    fileSystemTypeList.clear();
    fsList.clear();
    struct mntent *me;
//     if(fileName == "/etc/mtab") {
    FILE *mntfp = setmntent( fileName.latin1(), "r" );
    if ( mntfp ) {
        while ( (me = getmntent( mntfp )) != 0 ) {
            QString deviceName = me->mnt_fsname;
            QString filesystemType = me->mnt_type;
            if(deviceName != "none") {
                if( fsList.contains(filesystemType) == 0
                    & filesystemType.find("proc",0,TRUE) == -1
                    & filesystemType.find("cramfs",0,TRUE) == -1
                    & filesystemType.find("auto",0,TRUE) == -1)
                fsList << filesystemType;
                deviceList << deviceName;
                qDebug(deviceName+"::"+filesystemType);
                fileSystemTypeList << deviceName+"::"+filesystemType;
            }
        }
    }
    endmntent( mntfp );
//  } else if(fileName == "/etc/fstab") {
//    QFile f("/etc/fstab");
//   if ( f.open(IO_ReadOnly) ) {
//     QTextStream t (&f);
//     QString s;
//     while (! t.eof()) {
//         s=t.readLine();
//         s=s.simplifyWhiteSpace();
//         if ( (!s.isEmpty() ) && (s.find(" ")!=0) ) {
// // = me->mnt_fsname;
//             QString filesystemType = me->mnt_type;
//             QString deviceName = s.left(0,s.find(BLANK) ); 
//             s=s.remove(0,s.find(BLANK)+1 ); // devicename
          
//             s=s.remove(0,s.find(BLANK)+1 ); // mountpoint
//             QStringt mountPoint= s.left(0,s.find(BLANK) ); 
//             s=s.remove(0,s.find(BLANK)+1 ); // fs
//             QString filesystemType= s.left(0,s.find(BLANK) );           
//         }
//     }
//  }
//  f.close();
// }
}

QString FormatterApp::getFileSystemType(const QString &currentText) {

    parsetab("/etc/mtab"); //why did TT forget filesystem type?

    for ( QStringList::Iterator it = fileSystemTypeList.begin(); it != fileSystemTypeList.end(); ++it ) {
        QString temp = (*it);
        if( temp.find( currentText,0,TRUE) != -1) {
            return temp.right( temp.length() - temp.find("::",0,TRUE) - 2);
//                qDebug(fsType);
        }
    }
    return "";
}

bool FormatterApp::doFsck() {

  Output *outDlg;
      QString selectedDevice;
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_EBX) // lets test on something cheap
    selectedDevice = deviceComboBox->currentText();
#else
//    currentText  = diskDevice  = "/dev/fd0";
  QString  umountS = "umount -v /floppy 2>&1";
  QString  remountS = "mount -v /floppy 2>&1";
  selectedDevice ="/dev/fd0";
  
#endif

    QString fsType = getFileSystemType((const QString &)selectedDevice);
    QString cmd;
    qDebug( selectedDevice +" "+ fsType);
    if(fsType == "vfat") cmd = "dosfsck -vy ";
    if(fsType == "ext2") cmd = "e2fsck -cpvy ";
    cmd += selectedDevice + " 2>&1";

          outDlg = new Output(this, tr("Formatter Output"),FALSE);
          outDlg->showMaximized();
          outDlg->show();
          qApp->processEvents();
          FILE *fp;
          char line[130];
          outDlg->OutputEdit->append( tr("Trying to umount."));
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);

          sleep(1);
//          qDebug("Command is "+umountS);
          fp = popen(  (const char *) umountS, "r");
//          qDebug("%d", fp);
          if ( !fp ) {
              qDebug("Could not execute '" + umountS + "'!\n" +(QString)strerror(errno)); 
              QMessageBox::warning( this, tr("Formatter"), tr("umount failed!"), tr("&OK") );
              pclose(fp);             
              return false;
          } else {
//               outDlg->OutputEdit->append( currentText + tr("\nhas been successfully umounted."));
//             outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              while ( fgets( line, sizeof line, fp)) {
                  if( ((QString)line).find("busy",0,TRUE) != -1) {
                      qDebug("Could not find '" + umountS); 
                      QMessageBox::warning( this, tr("Formatter"), tr("Could not umount.\nDevice is busy!"), tr("&OK") );
                      pclose(fp);             
              return false;
                  } else {
                      QString lineStr = line;
                      lineStr=lineStr.left(lineStr.length()-1);
                      outDlg->OutputEdit->append(lineStr);
                      outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
                  }
              }
          }
          pclose(fp);             
/////////////////////////////////////
          fp = popen(  (const char *) cmd, "r"); 
          while ( fgets( line, sizeof line, fp)) {
              if( ((QString)line).find("No such device",0,TRUE) != -1) {
                  qDebug("No such device '" + umountS); 
                  QMessageBox::warning( this, tr("Formatter"), tr("No such device!"), tr("&OK") );
                  pclose(fp);             
//               outDlg->OutputEdit->append("No such device");
//             outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              return false;
              } else {
                  QString lineStr = line;
                  lineStr=lineStr.left(lineStr.length()-1);
                  outDlg->OutputEdit->append(lineStr);
                  outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
              }
          }
          outDlg->OutputEdit->append(tr("You can now close the output window."));
          outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
//           outDlg->OutputEdit->append( currentText + tr("\nhas been successfully formatted."));
//           outDlg->OutputEdit->setCursorPosition(outDlg->OutputEdit->numLines() + 1,0,FALSE);
          pclose(fp);             

/////////////////////////////////////////    

    return true;
}

bool FormatterApp::doFsckCheck() {

    return FALSE;
}

int FormatterApp::formatCheck(const QString &deviceStr) {

    return -1;
}

int FormatterApp::runCommand(const QString &command) {

    return -1;
}
