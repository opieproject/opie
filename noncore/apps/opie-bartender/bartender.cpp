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

#include <qtoolbar.h>
#include <qmenubar.h>
//#include <opie2/colorpopupmenu.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>

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
    fileMenu->insertItem(tr("Find by Alcohol"));

    QPopupMenu *editMenu;
    editMenu = new QPopupMenu( this);
    menuBar->insertItem( tr("Edit"), editMenu );
    editMenu->insertItem(tr("edit"));

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
    if(QDir("db").exists()) {
        dbFile.setName( "db/drinkdb.txt");
    } else
        dbFile.setName( QPEApplication::qpeDir()+"/etc/bartender/drinkdb.txt");
    initDrinkDb();
}

Bartender::~Bartender() {
}

/*
this happens right before exit  */
void Bartender::cleanUp() {
   dbFile.close();

}

void Bartender::initDrinkDb() {

    if(!dbFile.isOpen())
    if ( !dbFile.open( IO_ReadOnly)) {
        QMessageBox::message( (tr("Note")), (tr("Drink database not opened sucessfully.\n")) );
        return;
    }
    fillList();
}

void Bartender::fillList() {
    dbFile.at(1);
    DrinkView->clear();
    int i=0;
    QListViewItem * item ;
    QTextStream t( &dbFile);
    QString s;
    while ( !t.eof()) {
        s = t.readLine();
        if(s.find("#",0,TRUE) != -1) {
//            qDebug(s.right(s.length()-2));
            item= new QListViewItem( DrinkView, 0 );
            item->setText( 0, s.right(s.length()-2));
            i++;
        }
    }
    qDebug("there are currently %d of drinks", i);
}

void Bartender::fileNew() {

    New_Drink *newDrinks;
    newDrinks = new New_Drink(this,"New Drink....", TRUE);
    QString newName, newIng;
    QPEApplication::execDialog( newDrinks );
    newName = newDrinks->LineEdit1->text();
    newIng= newDrinks->MultiLineEdit1->text();

    if(dbFile.isOpen())
        dbFile.close();
    if ( !dbFile.open( IO_WriteOnly| IO_Append)) {
        QMessageBox::message( (tr("Note")), (tr("Drink database not opened sucessfully.\n")) );
        return;
    }
    if(newDrinks ->result() == 1 ) {
        QString newDrink="\n# "+newName+"\n";
        newDrink.append(newIng+"\n");
        qDebug("writing "+newDrink);
        dbFile.writeBlock( newDrink.latin1(), newDrink.length());
        clearList();
        dbFile.close();

        initDrinkDb();
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
    dbFile.at(0);
    Show_Drink *showDrinks;
    QString myDrink=item->text(0);
    showDrinks = new Show_Drink(this, myDrink, TRUE);
    QTextStream t( &dbFile);

    QString s, s2;
    while ( !t.eof()) {
        s = t.readLine();
        if(s.find( myDrink, 0, TRUE) != -1) {
            for(int i=0;s2.find( "#", 0, TRUE) == -1;i++) {
                s2 = t.readLine();
                if(s2.find("#",0,TRUE) == -1 || dbFile.atEnd() ) {
//                    qDebug(s2);
                    showDrinks->MultiLineEdit1->append(s2);
                }
                if( dbFile.atEnd() ) break;
            }
        }
    }
    QPEApplication::execDialog( showDrinks );

    if(showDrinks ->result() ==0) {
       doEdit();
    }
    delete showDrinks;
}

void Bartender::askSearch() {
    switch ( QMessageBox::warning(this,tr("Find"),tr("Search by drink name\n")+
                                  "\nor alcohol ?"
                                  ,tr("Drink Name"),tr("Alcohol"),0,0,1) ) {
      case 0:
          doSearchByName();
          break;
      case 1:
          doSearchByDrink();
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
            if ( it.current()->text(0).find( searchForDrinkName, 0, TRUE) != -1 ) {
//                 qDebug( it.current()->text(0));
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

void Bartender::doSearchByDrink() {
//    if( DrinkView->currentItem() == NULL) return;
    QStringList searchList;
    QString searchForDrinkName, lastDrinkName, tempName;

    InputDialog *fileDlg;
    fileDlg = new InputDialog(this,tr("Find by Alcohol"),TRUE, 0);
    fileDlg->exec();
    if( fileDlg->result() == 1 ) {
        searchForDrinkName = fileDlg->LineEdit1->text();

        dbFile.at(0);
        QTextStream t( &dbFile);

        QString s, s2;
        while ( !t.eof()) {
            s = t.readLine();
            if(s.find("#",0,TRUE) != -1) {
                lastDrinkName=s.right(s.length()-2);
//                qDebug("last drink name "+lastDrinkName);
            }
            else if( s.find( searchForDrinkName ,0, FALSE) != -1 && lastDrinkName != tempName ) {
//                qDebug("appending "+lastDrinkName);
                searchList.append( lastDrinkName);
                tempName=lastDrinkName;
            }
//            if( dbFile.atEnd() ) break;

        } //oef
        if(searchList.count() >0)
            showSearchResult(searchList);
        else
            QMessageBox::message(tr("Search"),tr("Sorry no results for\n")+ searchForDrinkName);
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
//                qDebug( it2.current()->text(0));
            showDrink(it2.current());
        }
    }
delete  searchDlg;
}

void Bartender::doEdit() {
   if(DrinkView->currentItem() == NULL) {
    fileNew();
   }

   QString myDrink;
    myDrink= DrinkView->currentItem()->text(0);
    dbFile.at(0);
    int foundAt=0;
    New_Drink *newDrinks;
    newDrinks = new New_Drink(this,"Edit Drink....", TRUE);
    QString newName, newIng;
    QPEApplication::showDialog( newDrinks );
    QTextStream t( &dbFile);

    QString s, s2;
    while ( !t.eof()) {
        s = t.readLine();
        if(s.find( myDrink, 0, TRUE) != -1) {
            foundAt = dbFile.at() - (s.length()+1);
            for(int i=0;s2.find( "#", 0, TRUE) == -1;i++) {
                s2 = t.readLine();
                if(s2.find("#",0,TRUE) == -1 || dbFile.atEnd() ) {
//                    qDebug(s2);
                    newDrinks->MultiLineEdit1->append(s2);
                    newDrinks->LineEdit1->setText(myDrink);
                }
                if( dbFile.atEnd() ) break;
            }
        }
    }
    newDrinks->exec();
    newName = newDrinks->LineEdit1->text();
    newIng= newDrinks->MultiLineEdit1->text();

    if( newDrinks ->result() == 1 ) {
        if(dbFile.isOpen())
            dbFile.close();
        if ( !dbFile.open( IO_ReadWrite )) {
            QMessageBox::message( (tr("Note")), (tr("Drink database not opened sucessfully.\n")) );
            return;
        }
        int fd = dbFile.handle();
        lseek( fd, foundAt, SEEK_SET);

//        dbFile.at( foundAt);
#warning FIXME problems with editing drinks db
        ////////// FIXME write to user file
        QString newDrink="# "+newName+"\n";
        newDrink.append(newIng+"\n");
        qDebug("writing "+newDrink);
        dbFile.writeBlock( newDrink.latin1(), newDrink.length());
        clearList();

        dbFile.flush();

        initDrinkDb();
    }
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
    qDebug("Item %d", item);
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
          doSearchByDrink();

          break;

    }
}

void Bartender::editMenuActivated(int item) {
    qDebug("Item %d", item);
      /*
        edit -8
      */
    switch(item) {
      case -8:
          doEdit() ;
          break;

    }
}

