#include "memorydialog.h"
#include "memory.h"
#include <qmessagebox.h>

extern Memory *memory;

MemoryDialog::MemoryDialog () : QDialog ( 0, 0, TRUE )
{
    setCaption ( tr ( "Edit Memory" ) );

    listbox = new QListBox ( this, "listbox" );
    memory->displayMemoryItems ( listbox );
    listbox->clearSelection();

    secondline = new QHBox ( this );

    newbutton = new QPushButton ( secondline );
    newbutton->setPixmap( QPixmap ("/opt/QtPalmtop/pics/new.png") );
    connect ( newbutton, SIGNAL ( released() ), this, SLOT ( addItem() ) );

    editbutton = new QPushButton ( secondline );
    editbutton->setPixmap( QPixmap ("/opt/QtPalmtop/pics/edit.png") );
    connect ( editbutton, SIGNAL ( released() ), this, SLOT ( editItem() ) );

    deletebutton = new QPushButton( secondline );
    deletebutton->setPixmap( QPixmap ("/opt/QtPalmtop/pics/delete.png") );
    connect ( deletebutton, SIGNAL ( released() ), this, SLOT ( deleteItem() ) );

    lineedit = new QLineEdit ( this );

    layout = new QVBoxLayout ( this, 2, 2 );
    layout->addWidget ( listbox );
    layout->addWidget ( secondline );
    layout->addWidget ( lineedit );
}

MemoryDialog::~MemoryDialog()
  {
  }

void MemoryDialog::addItem ()
  {
    if ( lineedit->text().length() != 0 )
      {
        memory->addMemoryItem ( lineedit->text() );
        listbox->clear ();
        memory->displayMemoryItems ( listbox );
        listbox->clearFocus();
        listbox->clearSelection ();
        lineedit->clear();
      }
  }

void MemoryDialog::editItem ()
  {
    if ( listbox->currentItem() != -1 )
      {
        lineedit->setText ( listbox->currentText() );
        memory->deleteMemoryItem ( listbox->currentText() );
        listbox->clear ();
        memory->displayMemoryItems ( listbox );
        listbox->clearSelection();
      }
    else
      QMessageBox::warning ( this, "QashMoney", "Please select an item to edit." );
  }

void MemoryDialog::deleteItem ()
  {
    if ( listbox->currentItem() != -1 )
      {
        memory->deleteMemoryItem ( listbox->currentText() );
        listbox->clear ();
        memory->displayMemoryItems ( listbox );
        listbox->clearSelection();
      }
    else
      QMessageBox::warning ( this, "QashMoney", "Please select an item to delete." );
  }







