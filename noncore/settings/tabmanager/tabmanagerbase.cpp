/****************************************************************************
** Form implementation generated from reading ui file 'tabmanagerbase.ui'
**
** Created: Fri May 3 14:38:14 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "tabmanagerbase.h"

#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a TabManagerBase which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
TabManagerBase::TabManagerBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "TabManagerBase" );
    resize( 289, 228 ); 
    setCaption( tr( "Tab Manger" ) );
    TabManagerBaseLayout = new QVBoxLayout( this ); 
    TabManagerBaseLayout->setSpacing( 6 );
    TabManagerBaseLayout->setMargin( 0 );

    tabList = new TabListView( this, "tabList" );
    tabList->addColumn( tr( "Groups" ) );
    tabList->addColumn( tr( "Applications" ) );
    tabList->setRootIsDecorated( TRUE );
    TabManagerBaseLayout->addWidget( tabList );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TabManagerBase::~TabManagerBase()
{
    // no need to delete child widgets, Qt does it all for us
}

