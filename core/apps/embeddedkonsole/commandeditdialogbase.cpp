/****************************************************************************
** Form implementation generated from reading ui file 'commandeditdialogbase.ui'
**
** Created: Sun Feb 3 15:23:58 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "commandeditdialogbase.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include "playlistselection.h"
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a CommandEditDialogBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
CommandEditDialogBase::CommandEditDialogBase( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "CommandEditDialogBase" );
    resize( 196, 414 ); 
    setCaption( tr( "Commands" ) );
    CommandEditDialogBaseLayout = new QGridLayout( this ); 
    CommandEditDialogBaseLayout->setSpacing( 6 );
    CommandEditDialogBaseLayout->setMargin( 11 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( tr( "<B>Commands</B>:" ) );

    CommandEditDialogBaseLayout->addWidget( TextLabel1, 0, 0 );

    Layout2 = new QVBoxLayout; 
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    ToolButton1 = new QToolButton( this, "ToolButton1" );
    ToolButton1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, ToolButton1->sizePolicy().hasHeightForWidth() ) );
    Layout2->addWidget( ToolButton1 );

    ToolButton2 = new QToolButton( this, "ToolButton2" );
    ToolButton2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, ToolButton2->sizePolicy().hasHeightForWidth() ) );
    Layout2->addWidget( ToolButton2 );

    ToolButton3 = new QToolButton( this, "ToolButton3" );
    ToolButton3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, ToolButton3->sizePolicy().hasHeightForWidth() ) );
    Layout2->addWidget( ToolButton3 );

    ToolButton4 = new QToolButton( this, "ToolButton4" );
    ToolButton4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, ToolButton4->sizePolicy().hasHeightForWidth() ) );
    Layout2->addWidget( ToolButton4 );

    ToolButton5 = new QToolButton( this, "ToolButton5" );
    ToolButton5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, ToolButton5->sizePolicy().hasHeightForWidth() ) );
    Layout2->addWidget( ToolButton5 );

    CommandEditDialogBaseLayout->addLayout( Layout2, 1, 2 );
    QSpacerItem* spacer = new QSpacerItem( 21, 20, QSizePolicy::Fixed, QSizePolicy::Minimum );
    CommandEditDialogBaseLayout->addItem( spacer, 0, 2 );

    TextLabel1_2 = new QLabel( this, "TextLabel1_2" );
    TextLabel1_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, TextLabel1_2->sizePolicy().hasHeightForWidth() ) );
    TextLabel1_2->setText( tr( "<B>Suggested Commands</B>:" ) );

    CommandEditDialogBaseLayout->addWidget( TextLabel1_2, 2, 0 );

    m_PlayListSelection = new PlayListSelection( this, "m_PlayListSelection" );
    m_PlayListSelection->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, m_PlayListSelection->sizePolicy().hasHeightForWidth() ) );

    CommandEditDialogBaseLayout->addMultiCellWidget( m_PlayListSelection, 1, 1, 0, 1 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 30, QSizePolicy::Minimum, QSizePolicy::Fixed );
    CommandEditDialogBaseLayout->addItem( spacer_2, 2, 1 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    CommandEditDialogBaseLayout->addItem( spacer_3, 2, 2 );

    m_SuggestedCommandList = new QListView( this, "m_SuggestedCommandList" );

    CommandEditDialogBaseLayout->addMultiCellWidget( m_SuggestedCommandList, 3, 3, 0, 1 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CommandEditDialogBase::~CommandEditDialogBase()
{
    // no need to delete child widgets, Qt does it all for us
}

