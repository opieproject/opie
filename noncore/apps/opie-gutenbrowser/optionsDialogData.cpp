/***************************************************************************
                          optionsDialogData.cpp  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <qpixmap.h>
#include "optionsDialog.h"
#include "fontDialog.h"

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

void optionsDialog::initDialog()
{
//    QWidget *d = QApplication::desktop();
//    int width = d->width();
//    int height = d->height();

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing(2);
    layout->setMargin(2);

    tabWidget = new QTabWidget( this, "tabWidget" );
    layout->addMultiCellWidget( tabWidget, 0, 0, 0, 3);

    Widget2 = new QWidget( tabWidget, "Widget2" );

    QGridLayout *layout2 = new QGridLayout(Widget2 );
    layout2->setSpacing(2);
    layout2->setMargin(2);

//    Http_CheckBox = new QCheckBox( Widget2, "Http_CheckBox" );

    QLabel *downLabel= new QLabel(Widget2,"Label1");
    downLabel->setText("Download Directory:");
    downLabel->setMaximumHeight(30);

    downloadDirEdit = new QLineEdit(Widget2,"downloadDir");

//      ComboBoxStyle = new QComboBox( FALSE, Widget2, "ComboBoxStyle" );
//      ComboBoxStyle->insertItem( tr( "styleMetal") );
//      ComboBoxStyle->insertItem( tr( "styleWindows") );
//      ComboBoxStyle->insertItem( tr( "default style") );

    cb_queryExit = new QCheckBox( Widget2, "cb_queryExit" );
    cb_queryExit->setText("Query before exit.");

    useWordWrap_CheckBox = new QCheckBox( Widget2, "WordWrap_CheckBox" );
    useWordWrap_CheckBox->setText("Use Word Wrap");
//    Http_CheckBox->setText("Use pg index instead of ftp library index" ) ;
    Ftp_CheckBox = new QCheckBox( Widget2, "Ftp_CheckBox" );
    Ftp_CheckBox->setText("Use FTP");
    Ftp_CheckBox->hide();

    layout2->addMultiCellWidget( downLabel, 0, 0, 0, 2);
//		downloadDirEdit->setMaximumWidth( 200);
    layout2->addMultiCellWidget( downloadDirEdit, 1, 1, 0, 0);
    //    layout2->addMultiCellWidget( ComboBoxStyle, 3, 3, 0, 0);
    layout2->addMultiCellWidget( cb_queryExit, 2, 2, 0, 1);
    layout2->addMultiCellWidget( useWordWrap_CheckBox, 3, 3, 0, 1);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout2->addItem( spacer, 4, 0 );
//    downloadDirEdit->hide();

//////////////////////////////////////////////////////////
    tabWidget->insertTab( Widget2, tr( "Main" ) );

    Widget3 = new QWidget( tabWidget, "Widget3" );

    QGridLayout *layout3 = new QGridLayout(Widget3 );
    layout3->setSpacing(2);
    layout3->setMargin(4);

    TextLabel3 = new QLabel( Widget3, "TextLabel3" );
  //  TextLabel3->setProperty( "text", tr( "Current ftp server:/n" ) );
    ftp_QListBox_1 = new QListBox( Widget3, "ftp_QListBox_1" );
    ftp_DownloadButton = new QPushButton( Widget3, "ftp_DownloadButton" );

//////////////////////////////////////////////////////////
    tabWidget->insertTab( Widget3, tr( "FTP" ) );
    tab = new QWidget( tabWidget, "tab" );

    TextLabel3_3 = new QLabel( tab, "TextLabel3_3" );

//     TextLabel4 = new QLabel( tab, "TextLabel4" );
//     TextLabel5 = new QLabel( tab, "TextLabel5" );

    PushButton_Browse = new QPushButton( tab, "PushButton_Browse" );

    ComboBox1 = new QComboBox( FALSE, tab, "ComboBox1" );

    ComboBox1->insertItem( tr( "http://sailor.gutenberg.org" ) );
    ComboBox1->insertItem( tr( "http://www.prairienet.org/pg" ) );

    http_ListBox1 = new QListBox( tab, "http_ListBox1" );

//////////////////////////////////////////////////////////
    tabWidget->insertTab( tab, tr( "HTTP" ) );

    QGridLayout *layout4 = new QGridLayout(tab );
    layout4->setSpacing(2);
    layout4->setMargin(2);

    fontDlg = new FontDialog( tabWidget,"FontDialog");
//    fontWidget= new QWidget( tabWidget, "fontWidget" );
    tabWidget->insertTab( fontDlg,"Font");
//    fontDlg = new FontDialog( fontWidget,"FontDialog");
//      fontDlg->show();

    initConnections();

    ftp_DownloadButton->setText("New List");
    PushButton_Browse->setText("Browse");

    local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
//    config = new CConfigFile( local_library+"gutenbrowserrc");
    doOptions();

    layout3->addMultiCellWidget( ftp_DownloadButton, 0, 0, 1, 2);
    layout3->addMultiCellWidget( TextLabel3, 1, 1, 0, 1);
    layout3->addMultiCellWidget( ftp_QListBox_1, 2, 2, 0, 2);

    TextLabel3_3->setMaximumHeight(50);
    layout4->addMultiCellWidget( PushButton_Browse, 0, 0, 1, 2);
    layout4->addMultiCellWidget( TextLabel3_3, 1, 1, 0, 1);
    layout4->addMultiCellWidget( http_ListBox1, 2, 3, 0, 2);
    layout4->addMultiCellWidget( ComboBox1, 4, 4, 0, 2);
    setCaption(tr("Gutenbrowser Options"));
}

void optionsDialog::initConnections() {
      // signals and slots connections
//    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( OnExit()  ) );
//    connect( fontChangeButton, SIGNAL( clicked() ), this, SLOT( changeFonts() ) );

//  connect( s_Browser, SIGNAL( activated(const QString&)), SLOT( goBrowser(const QString&) ) );
//    connect( Http_CheckBox, SIGNAL( clicked()), SLOT( httpCheckB()) );
//    connect( downloadDirEdit, SIGNAL(textChanged(const QString &)),SLOT( lineEditChanged(const QString &) ));

//    connect( Ftp_CheckBox, SIGNAL( clicked()), SLOT( FtpCheckB()) );
//    downloadLineEdit

// ftpsiteDlg
    connect( ftp_DownloadButton, SIGNAL( released() ),this, SLOT( getSite() ));
    connect( ftp_QListBox_1, SIGNAL( clicked (  QListBoxItem *)),this, SLOT( getSelection(  QListBoxItem *)) );

//browserDialog
    connect( http_ListBox1, SIGNAL(highlighted ( int)),this, SLOT(select_title(int)) );
    connect( PushButton_Browse, SIGNAL( clicked() ), SLOT( BrowseSelected() ) );
//    connect( ComboBox1,SIGNAL(activated(int)),this,SLOT( setHttp(int)));

//      connect( tabWidget,SIGNAL(currentChanged(QWidget *tabber)),SLOT( onTab( QWidget * tabber)) );

    connect(cb_queryExit,SIGNAL(clicked()),SLOT( slotQueryExit()) );
			//    connect(ComboBoxStyle,SIGNAL(activated(int)),this,SLOT(setStyle(int)));
		connect( useWordWrap_CheckBox,SIGNAL( toggled(bool)),SLOT( slotWordWrap(bool)) );
}

void optionsDialog::listBoxMainSel( int index) {
    tabWidget->setCurrentPage( index);
}
