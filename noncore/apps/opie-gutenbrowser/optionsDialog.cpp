/***************************************************************************
                          optionsDialog.cpp  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "optionsDialog.h"
#include "fontDialog.h"
//#include "ftpsitedlg.h"
//#include "browserDialog.h"
//#define Inherited  QDialog

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>

/* QT */
#include <qfont.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#ifndef Q_WS_QWS
#include <qwhatsthis.h>
#endif

/* STD */
// #include <unistd.h>
// #include <stdio.h>
// #include <stdlib.h>

/*
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
optionsDialog::optionsDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "optionsDialog" );
    changedFonts=FALSE;
    //    styleChanged=FALSE;
//    connect( this,SIGNAL( aboutToQuit()),SLOT( OnExit()) );
    b_qExit=TRUE;
    initDialog();

    ftpSiteDlg();
    BrowserDlg();
    FontDialog();
}

optionsDialog::~optionsDialog()
{
//odebug << "OnExit()" << oendl;
//    writeConfig();
}

/// optionsDialog
void optionsDialog::doOptions() {

//    config.read();
    getConfig();
}

void optionsDialog::getConfig()
{
    Config config("Gutenbrowser");
    config.setGroup( "FTPsite" );  // ftp server config
    ftp_host=config.readEntry("SiteName", "sailor.gutenberg.org");
    ftp_base_dir= config.readEntry("base",  "/pub/gutenberg");

    TextLabel3->setText( "Current ftp server:\n"+ftp_host/* + ftp_base_dir */);

    config.setGroup( "Proxy" ); // use ftp/http config
    if( config.readEntry("IsChecked", "FALSE") == "TRUE") {
        Ftp_CheckBox->setChecked( FALSE);
    } else {
        Ftp_CheckBox->setChecked( TRUE);
    }

    config.setGroup( "HttpServer" );
    QString s_http = config.readEntry("Preferred", "http://sailor.gutenbook.org");
    if( s_http == "http://sailor.gutenbook.org")
        ComboBox1->setCurrentItem(0);
    else
        ComboBox1->setCurrentItem(1);

    config.setGroup( "Browser" );
    brow = config.readEntry("Preferred", "Opera");
    browserName=brow;

    TextLabel3_3->setText( "Current http browser:\n"+brow );

    config.setGroup( "Geometry" );

    //    bool ok2;
    QString temp;
//      config.setGroup( "StyleIze" );
//      temp = config.readEntry( "Style", "0");
//      int i_style=temp.toInt( &ok2,10);
//  #ifndef Q_WS_QWS

//          if( i_style==0){ styleInt=0; ComboBoxStyle->setCurrentItem(0);}//"styleMetal"));
//          if( i_style==1) { styleInt=1; ComboBoxStyle->setCurrentItem(1);}//"styleWindows") );
//          if( i_style==2) { styleInt=2; ComboBoxStyle->setCurrentItem(2);}//"stylePlatinum"));
//        if( i_style==2){ styleInt=2; ComboBoxStyle->setCurrentItem(2);}//"styleCDE") );
//        if( i_style==3) { styleInt=3; ComboBoxStyle->setCurrentItem(3);}//"styleMotif") );
//        if( i_style==4) { styleInt=4; ComboBoxStyle->setCurrentItem(4);}//"styleMotifPlus"));
//  //      if( i_style==5) { styleInt=5; ComboBoxStyle->setCurrentItem(5);}//"styleWood") );
//        if( i_style==6) { styleInt=6; ComboBoxStyle->setCurrentItem(6);}//"styleSgi") );
//        if( i_style==7) { styleInt=7; ComboBoxStyle->setCurrentItem(7);}//"stylePlatinumPlus") );
//      if( i_style==9){ styleInt=9; ComboBoxStyle->setCurrentItem(9);}//"styleLace"));
//      if( i_style==10){ styleInt=10; ComboBoxStyle->setCurrentItem(10);}//"styleLacePlus"));
//  #endif
    config.setGroup("General");
    QString qExit=config.readEntry("queryExit","TRUE");
    if(qExit=="TRUE") {
        cb_queryExit->setChecked(TRUE);
        odebug << "Config Setting    b_qExit=TRUE;" << oendl;
        b_qExit=TRUE;
    } else {
        cb_queryExit->setChecked(FALSE);
        odebug << "ConfigSetting    b_qExit=FALSE;" << oendl;
        b_qExit=FALSE;
    }


      useWordWrap_CheckBox->setChecked( config.readBoolEntry("WordWrap", 1));
      QString downDir=config.readEntry( "DownloadDirectory",local_library);
      downloadDirEdit->setText(downDir);
} // end getConfig

//  //  void optionsDialog::setStyle( int index) {

//  //      odebug << "Set style" << index << "" << oendl;
//  //  //int styleInt= index;
//  //   styleChanged=TRUE;
//  //        if( ComboBoxStyle->currentItem()==0) styleInt=0;//"styleMetal"));
//  //        if( ComboBoxStyle->currentItem()==1) styleInt=1;//"styleWindows") );
//  //        if( ComboBoxStyle->currentItem()==2) styleInt=2;//"stylePlatinum"));
//  //  //      if( ComboBoxStyle->currentItem()==2) styleInt=2;//"styleCDE") );
//  //  //      if( ComboBoxStyle->currentItem()==3) styleInt=3;//"styleMotif") );
//  //  //      if( ComboBoxStyle->currentItem()==4) styleInt=4;//"styleMotifPlus"));
//  //  //  //    if( ComboBoxStyle->currentItem()==5) styleInt=5;//"styleWood") );
//  //  //      if( ComboBoxStyle->currentItem()==6) styleInt=6;//"styleSgi") );
//  //  //      if( ComboBoxStyle->currentItem()==7) styleInt=7;//"stylePlatinumPlus") );
//  //  //    if( ComboBoxStyle->currentItem()==9) styleInt=9;//"styleLace"));
//  //  //    if( ComboBoxStyle->currentItem()==10) styleInt=10;//"styleLacePlus"));
//  //      QString temp;
//  //      temp.setNum(styleInt, 10);
//  //      Config config("Gutenbrowser");
//  //      config.setGroup( "StyleIze" );
//  //      config.writeEntry( "Style", temp);
//  //      config.write();
//  //  }

void optionsDialog::slotQueryExit()
{
    Config config("Gutenbrowser");
    config.setGroup("General");
    odebug << "writing queryExit" << oendl;
    if(cb_queryExit->isChecked()) {
        b_qExit=TRUE;
        config.writeEntry("queryExit","TRUE");
        odebug << "WritingConfig queryExit=TRUE" << oendl;
    } else {
        b_qExit=FALSE;
        config.writeEntry("queryExit","FALSE");
        odebug << "WritingConfig queryExit=FALSE" << oendl;
    }
    config.write();
}


//  void optionsDialog::slotUseSplit()
//  {

//  }

//  void optionsDialog::lineEditChanged(const QString &editText) {
//      odebug << editText << oendl;
//  }

void optionsDialog::changeFonts()
{
    changedFonts=TRUE;
}


void optionsDialog::slotWordWrap(bool b) {

  Config config("Gutenbrowser");
  config.setGroup("General");

  config.writeEntry("WordWrap", b);
  config.write();

}
