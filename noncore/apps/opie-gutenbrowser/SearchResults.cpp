/****************************************************************************
** Created: Sat Feb 24 23:35:01 2001
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
****************************************************************************/
#include "SearchResults.h"
#include "gutenbrowser.h"
#include "LibraryDialog.h"

#include <qpe/qpeapplication.h>
#include <stdio.h>
#include <qstrlist.h>
#include <qclipboard.h>

#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
//#include <qtooltip.h>
//#include <qwhatsthis.h>

SearchResultsDlg::SearchResultsDlg( QWidget* parent,  const char* name, bool modal, WFlags fl, QStringList  stringList )
        : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "SearchResultsDlg" );

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);
    
    ListBox1 = new QListBox( this, "ListBox1" );
    outPutLabel=new QLabel( this, "outPutLabel" );
    statusLabel=new QLabel( this, "StatusLabel" );
    QString local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";

    buttonOk = new QPushButton(this,"buttonOk");
    buttonCancel = new QPushButton(this,"buttonCancel");

    layout->addMultiCellWidget( buttonOk, 0, 0, 0, 0);
    layout->addMultiCellWidget( buttonCancel, 0, 0, 1, 1);

    layout->addMultiCellWidget( ListBox1, 1, 4, 0, 1);
    
    outPutLabel->setMaximumHeight(30);
    statusLabel->setMaximumHeight(30);

    layout->addMultiCellWidget( outPutLabel, 5, 5, 0, 1);
    layout->addMultiCellWidget( statusLabel, 6, 6, 0, 1);
    

    setCaption( tr( "Search Results"  ) );
    buttonOk->setText( tr( "&Download"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonCancel->setText( tr( "&Cancel"  ) );
    buttonCancel->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    ListBox1->setSelectionMode( QListBox::Single );
    ListBox1->setMultiSelection(true);
    statusLabel->setText( "Double click a title to begin downloading." );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( dwnLd() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( ListBox1, SIGNAL( doubleClicked(QListBoxItem *)),  SLOT( slotListClicked(QListBoxItem * )) );

    ListBox1->insertStringList( stringList);
    numListItems= ListBox1->numRows();
    QString strOut;

    outPutLabel->setText( "Number of results: "+ strOut.sprintf("%d", numListItems) );

//    printf( "Sucessfully entered SearchResults Dialog\n");
// selText = resultStr;
}

SearchResultsDlg::~SearchResultsDlg()
{
}

void SearchResultsDlg::slotListClicked(QListBoxItem *it) {

    resultsList.append(it->text() );
//    resultsList.append(ListBox1->text( 0));
//    selText = ListBox1->currentText();
    accept();
}

void SearchResultsDlg::dwnLd() {

    for(unsigned int ji=0; ji< ListBox1->count() ; ji++ ) {
        if( ListBox1->isSelected( ji) )
            resultsList.append(ListBox1->text( ji));
    }
    accept();
}

void SearchResultsDlg::downloadButtonSlot() {

}
