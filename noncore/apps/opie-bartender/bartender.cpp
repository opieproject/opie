/****************************************************************************
**
** Created: Sat Jul 20 08:10:53 2002
**      by:  L.J. Potter <ljp@llornkcor.com>
**     copyright            : (C) 2002 by ljp
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/

#include "bartender.h"
#include "showdrinks.h"
#include "inputDialog.h"
#include "searchresults.h"
#include "bac.h"
#include "drinkdata.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
using namespace Opie::Core;

/* QT */
#include <qlineedit.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qmultilineedit.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qaction.h>
#include <qheader.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtoolbar.h>
#include <qmenubar.h>

/* STD */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

Bartender::Bartender( QWidget* parent,  const char* name, WFlags fl )
    : QMainWindow( parent, name, fl ) {
    if ( !name )
  setName( "Bartender" );
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);
    connect( qApp,SIGNAL( aboutToQuit()),SLOT( cleanUp()) );

    setCaption( tr( "Bartender" ) );

    setToolBarsMovable( FALSE );
        
    ToolBar1 = new QToolBar( this, "ToolBar1" );
    ToolBar1->setFixedHeight(22);
    layout->addMultiCellWidget( ToolBar1, 0, 0, 0, 4 );

    QMenuBar *menuBar = new QMenuBar( ToolBar1 );
    QPopupMenu *fileMenu;
    fileMenu = new QPopupMenu( this);
    menuBar->insertItem( tr("File"), fileMenu );

    fileMenu->insertItem(tr("New Drink"));
    fileMenu->insertItem(tr("Open Drink"));
    fileMenu->insertItem(tr("Find by Drink Name"));
    fileMenu->insertItem(tr("Find by Ingredient"));

    QPopupMenu *editMenu;
    editMenu = new QPopupMenu( this);
    menuBar->insertItem( tr("Edit"), editMenu );
    editMenu->insertItem(tr("Edit Drink"));

    connect( fileMenu, SIGNAL( activated(int) ), this, SLOT( fileMenuActivated(int) ));
    connect( editMenu, SIGNAL( activated(int) ), this, SLOT( editMenuActivated(int) ));


    QAction *a = new QAction( tr( "New" ), Resource::loadPixmap( "new" ), "New", 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( ToolBar1 );

    a = new QAction( tr( "Open" ), Resource::loadPixmap( "bartender/bartender_sm" ), "open", 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( openCurrentDrink() ) );
    a->addTo( ToolBar1 );

    a = new QAction( tr( "Find" ), Resource::loadPixmap( "find" ), "Find", 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( askSearch() ) );
    a->addTo( ToolBar1 );

    a = new QAction( tr( "Edit" ), Resource::loadPixmap( "edit" ),"Edit", 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( doEdit() ) );
    a->addTo( ToolBar1 );

    QPushButton *t;
    t= new QPushButton( "BAC", ToolBar1, "bacButtin");
    connect( t, SIGNAL( clicked() ), this, SLOT( doBac() ) );

    DrinkView = new QListView( this, "DrinkView" );
    DrinkView->addColumn( tr( "Name of Drink" ) );
//    DrinkView->setRootIsDecorated( TRUE );
    DrinkView->header()->hide();

    QPEApplication::setStylusOperation( DrinkView->viewport(),QPEApplication::RightOnHold);

    connect(DrinkView, SIGNAL( doubleClicked(QListViewItem*)),this,SLOT(showDrink(QListViewItem*)));
    connect(DrinkView, SIGNAL( mouseButtonPressed(int,QListViewItem*,const QPoint&,int)),
            this,SLOT( showDrink(int,QListViewItem*,const QPoint&,int)));

    layout->addMultiCellWidget( DrinkView, 1, 2, 0, 4 );
    if(QDir("db").exists())
        drinkDB.setFile("db/drinkdb.txt");
    else
        drinkDB.setFile(QPEApplication::qpeDir()+"etc/bartender/drinkdb.txt");
    
    initDrinkDb();

    DrinkView->setFocus();
}

Bartender::~Bartender() {
}

/*
this happens right before exit  */
void Bartender::cleanUp() {

}

void Bartender::initDrinkDb() {
    if(drinkDB.read())
        fillList();
    else
        QMessageBox::message( (tr("Note")), (tr("Drink database not opened sucessfully.\n")) );
}

void Bartender::fillList() {
    QString lastName;
    if( DrinkView->currentItem() != NULL )
        lastName = DrinkView->currentItem()->text(0);
    else
        lastName = "";

    DrinkView->clear();
    int i=0;
    QListViewItem * item ;
    QString s;
    
    DrinkList::Iterator it = drinkDB.getBegin();
    while ( it != drinkDB.getEnd() ) {
        item= new QListViewItem( DrinkView, 0 );
        item->setText( 0, (*it).getName());
        i++;
        ++it;
    }
    
    if( lastName != "" ) {
        QListViewItemIterator it( DrinkView );
        for ( ; it.current(); ++it ) {
            if ( it.current()->text(0) == lastName ) {
                DrinkView->setCurrentItem(it.current());
                break;
            }
        }
    }
    else if( DrinkView->childCount() > 0)
        DrinkView->setCurrentItem(DrinkView->firstChild());
    
    odebug << "there are currently " << i << " of drinks" << oendl; 
}

void Bartender::fileNew() {

    New_Drink *newDrinks;
    newDrinks = new New_Drink(this,"New Drink", TRUE);
    QString newName, newIng;
    QPEApplication::execDialog( newDrinks );
    newName = newDrinks->LineEdit1->text();
    newIng= newDrinks->MultiLineEdit1->text();

    if(newDrinks ->result() == 1 ) {
        drinkDB.addDrink(newName, newIng);
        if(!drinkDB.writeChanges()) {
            QMessageBox::message( (tr("Note")), (tr("Failed to write to drink database!\n")) );
        }
        fillList();
    }
    delete newDrinks;
}

void Bartender::showDrink(int mouse, QListViewItem * item, const QPoint&, int) {
    switch (mouse) {
      case 1:
//          showDrink(item);
          break;
      case 2:
          showDrink(item);
          break;
    }
}

void Bartender::showDrink( QListViewItem *item) {
    if(item==NULL) return;
    Show_Drink *showDrinks;
    QString myDrink=item->text(0);
    showDrinks = new Show_Drink(this, myDrink, TRUE);

    DrinkList::Iterator it = drinkDB.findDrink(myDrink);
    if(it != drinkDB.getEnd()) {
        showDrinks->MultiLineEdit1->setText((*it).getIngredients());
    
        connect(showDrinks->editDrinkButton, SIGNAL(clicked()), this, SLOT(doEdit()));
        QPEApplication::execDialog( showDrinks );
    }

    delete showDrinks;
}

void Bartender::askSearch() {
    switch ( QMessageBox::warning(this,tr("Find"),tr("Search by drink name\n")+
                                  "\nor ingredient ?"
                                  ,tr("Drink Name"),tr("Ingredient"),0,0,1) ) {
      case 0:
          doSearchByName();
          break;
      case 1:
          doSearchByIngredient();
          break;
    };
}

/* search by name */
void Bartender::doSearchByName() {
//    if( DrinkView->currentItem() == NULL) return;
    QStringList searchList;
    QString searchForDrinkName;
    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Find by Drink Name"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        searchForDrinkName = fileDlg->LineEdit1->text();
        QListViewItemIterator it( DrinkView );
        for ( ; it.current(); ++it ) {
            if ( it.current()->text(0).find( searchForDrinkName, 0, FALSE) != -1 ) {
//                 odebug << it.current()->text(0) << oendl; 
                searchList.append(it.current()->text(0));
            }
        }
        if(searchList.count() >0)
            showSearchResult(searchList);
        else
            QMessageBox::message(tr("Search"),tr("Sorry no results for\n")+searchForDrinkName);
    }//end Inputdialog
    delete fileDlg;
}

void Bartender::doSearchByIngredient() {
//    if( DrinkView->currentItem() == NULL) return;
    QStringList searchList;
    QString searchForIngredient, lastDrinkName, lastDrinkIngredients;

    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Find by Ingredient"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        searchForIngredient = fileDlg->LineEdit1->text();

        DrinkList::Iterator it = drinkDB.getBegin();
        while ( it != drinkDB.getEnd() ) {
            lastDrinkName = (*it).getName();
            lastDrinkIngredients = (*it).getIngredients();
            
            if( lastDrinkIngredients.find( searchForIngredient ,0, FALSE) != -1 )
                searchList.append( lastDrinkName );
            
            ++it;
        }
        
        if(searchList.count() >0)
            showSearchResult(searchList);
        else
            QMessageBox::message(tr("Search"),tr("Sorry no results for\n")+ searchForIngredient);
    }
    delete fileDlg;
}

void Bartender::showSearchResult(QStringList &searchList) {
    QString result;
    Search_Results *searchDlg;

    searchList.sort();

    searchDlg = new Search_Results(this, "Search Results", TRUE);
    searchDlg->ListBox1->insertStringList( searchList,-1);
    QPEApplication::execDialog( searchDlg );

    if( searchDlg->result() == 1 ) {
        result= searchDlg->ListBox1->currentText();
    }
    QListViewItemIterator it2( DrinkView );
    for ( ; it2.current(); ++it2 ) {
        if ( it2.current()->text(0)== result ) {
//                odebug << it2.current()->text(0) << oendl; 
            showDrink(it2.current());
        }
    }
delete  searchDlg;
}

void Bartender::doEdit() {
    if(DrinkView->currentItem() == NULL) {
      fileNew();
      return;
    }

    QString myDrink;
    myDrink= DrinkView->currentItem()->text(0);
    
    New_Drink *newDrinks;
    newDrinks = new New_Drink(this,"Edit Drink", TRUE);
    QString newName, newIng;
    QPEApplication::showDialog( newDrinks );

    DrinkList::Iterator drinkItem = drinkDB.findDrink(myDrink);
    if(drinkItem != drinkDB.getEnd()) {
        newDrinks->LineEdit1->setText((*drinkItem).getName());
        newDrinks->MultiLineEdit1->setText((*drinkItem).getIngredients());
        
        newDrinks->exec();
        newName = newDrinks->LineEdit1->text();
        newIng= newDrinks->MultiLineEdit1->text();
    
        if( newDrinks ->result() == 1 ) {
            (*drinkItem).setName(newName);
            (*drinkItem).setIngredients(newIng);
            if(!drinkDB.writeChanges()) {
                QMessageBox::message( (tr("Note")), (tr("Failed to write to drink database!\n")) );
            }
            fillList();
        }
    }
    
    delete newDrinks;
}

void Bartender::clearList() {
    DrinkView->clear();
}

void Bartender::doBac() {
    BacDialog *bacDlg;
    bacDlg = new BacDialog(this,"BAC",TRUE);
    QPEApplication::execDialog( bacDlg );
    delete bacDlg;
}

void Bartender::openCurrentDrink() {
    if(DrinkView->currentItem() == NULL) return;
    showDrink(DrinkView->currentItem());
}

void Bartender::fileMenuActivated( int item) {
    odebug << "Item " << item << "" << oendl; 
    switch(item) {
      case -3: //        new -3
          fileNew();
          break;
      case -4://        open -4
          openCurrentDrink();
          break;
      case -5://        drink -5
          doSearchByName();

          break;
      case -6://        alcohol -6
          doSearchByIngredient();

          break;

    }
}

void Bartender::editMenuActivated(int item) {
    odebug << "Item " << item << "" << oendl; 
      /*
        edit -8
      */
    switch(item) {
      case -8:
          doEdit() ;
          break;

    }
}

