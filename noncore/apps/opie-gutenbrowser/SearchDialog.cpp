/****************************************************************************
** Created: Tue Aug 29 11:45:00 2000**/
//     copyright            : (C) 2000 -2004 by llornkcor
//     email                : ljp@llornkcor.com

#include "SearchDialog.h"
#include "SearchResults.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qpe/config.h>

/*This is just a single text entry dialog */
SearchDialog::SearchDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "SearchDialog" );
    Config cfg("Gutenbrowser");
    cfg.setGroup("General");
    QString lastSearch=cfg.readEntry("LastSearch","");

#warning FIXME
    // FIXME
    resize( 220,110);

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);

    QString local_library = (QDir::homeDirPath ()) +"/Applications/gutenbrowser/";
    TextLabel1 = new QLabel( this, "TextLabel1" );
    layout->addMultiCellWidget( TextLabel1, 0, 0, 0, 1);

    label1Str= "<P>Enter text to search etext for </P>" ;
    TextLabel1->setText( tr( label1Str) );

    SearchLineEdit = new QLineEdit( this, "SearchLineEdit" );
    layout->addMultiCellWidget( SearchLineEdit, 1, 1, 0, 1);

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( tr( "Sea&rch"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setToggleButton( TRUE);
    buttonOk->setDefault( TRUE );

    layout->addMultiCellWidget(buttonOk, 2, 2, 0, 0);

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel"  ) );
    buttonCancel->setAutoDefault( TRUE );
    layout->addMultiCellWidget(buttonCancel, 2, 2, 1, 1);
//    buttonCancel->setMaximumWidth(40);


    if( (QString)name !="Etext Search" )
        SearchLineEdit->setText(lastSearch);

    caseSensitiveCheckBox = new QCheckBox ( tr("Case Sensitive"), this );
    layout->addMultiCellWidget( caseSensitiveCheckBox, 3, 3, 0, 1);

      // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( byeBye() ) );

    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( closed() ) );
    SearchLineEdit->setFocus();
}

SearchDialog::~SearchDialog()
{
}

QString SearchDialog::get_text() {
  return SearchLineEdit->text();
}

bool SearchDialog::get_direction() {
  return false; //search forward
}

bool SearchDialog::case_sensitive() {
  return true;
}

bool SearchDialog::forward_search() {
  return true;
}

void SearchDialog::byeBye()
{

    searchString = get_text();
//    odebug << "Search string is "+searchString << oendl;
    Config cfg("Gutenbrowser");
    cfg.setGroup("General");
    cfg.writeEntry("LastSearch",searchString);

    QString thisName=name();
    if( thisName.find("Library Search", 0, TRUE) != -1) {
          //  searchString = SearchLineEdit->text();
        accept();
    } else {

        buttonOk->setDown(TRUE);

        emit search_signal();
        buttonOk->setDown(FALSE);
    }
}

void SearchDialog::closed()
{
    searchString = get_text();
//    odebug << "Search string is "+searchString << oendl;
    Config cfg("Gutenbrowser");
    cfg.setGroup("General");
    cfg.writeEntry("LastSearch",searchString);

    emit search_done_signal();
      //this->reject();
    this->hide();
}


void SearchDialog::setLabel(QString labelText)
{
    TextLabel1->setText( tr( label1Str+labelText) );
}
