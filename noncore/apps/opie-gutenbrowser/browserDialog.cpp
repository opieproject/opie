/****************************************************************************
copyright 2001 by L.J. Potter ljp@llornkcor.com
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com

****************************************************************************/

#include "optionsDialog.h"
//#include "fileBrowser.h"

#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>

#include <opie/ofiledialog.h>

void optionsDialog::BrowserDlg( )
{
//    setCaption( tr( "Choose Browser" ) );
  http_ListBox1->insertItem( tr( "Opera"));
  http_ListBox1->insertItem( tr( "Konqueror"));
  http_ListBox1->insertItem( tr( "wget"));
}

void optionsDialog::select_title(int) {
    browserName = http_ListBox1->currentText();
    Config config("Gutenbrowser");
    config.setGroup( "Browser" );
    printf("Brow is: "+browserName+"\n");
    config.writeEntry("Preferred", browserName);
    TextLabel3_3->setText( "Current http browser: "+browserName );
}

void optionsDialog::BrowseSelected() {
    QString fileName;
    Config cfg("Gutenbrowser");
    cfg. setGroup ( "View" );
    QString dir = cfg.readEntry("LastOpenDirectory", QPEApplication::documentDir());
    QMap<QString, QStringList> map;
    map.insert(tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );

    QString str = OFileDialog::getOpenFileName( 2, dir , QString::null, map);
    if( !str.isEmpty() && QFile(str).exists() && !QFileInfo(str).isDir() )  {
      fileName = str;
//      QStringList fileList=browseForFiles->fileList;
//      qDebug(selFile);
//      QStringList::ConstIterator f;
//      QString fileTemp,filer;
//      for (  f = fileList.begin(); f != fileList.end(); f++ ) {

//          fileTemp = *f;
//          fileTemp.right( fileTemp.length()-5);
//          fileName = fileTemp;
//          if( !fileName.isEmpty() ){
//              filer = fileName;
//          } else {
//              QString sMsg;
//              sMsg = "Error opening library filelist "+fileName;
//          }
        if ( !fileName.isNull() ) {                 // got a file name
              //   ...
        }
    }

}

void optionsDialog::setHttp(int index) {
    Config config("Gutenbrowser");    
    config.setGroup( "HttpServer" );
    qDebug("writing http server");
    if( index== 0) {
        config.writeEntry("Preferred", "http://sailor.gutenberg.org");
    } else {
        config.writeEntry("Preferred", "http://www.prairienet.org/pg");
    }

}
