/****************************************************************************
** Form implementation generated from reading ui file 'installdialog.ui'
**
** Created: Thu May 2 22:19:02 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "installdialog.h"

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


InstallDialog::InstallDialog( PackageManagerSettings* s, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
		settings = s;
    if ( !name )
			setName( "InstallDialog" );
    resize( 223, 269 ); 
    setCaption( tr( "Install" ) );
    InstallDialogLayout = new QGridLayout( this );
    InstallDialogLayout->setSpacing( 2 );
    InstallDialogLayout->setMargin( 2 );

  	ListViewPackages =  new PackageListView( this,"listViewPackages",settings );

    InstallDialogLayout->addWidget( ListViewPackages, 0, 0 );

    GroupBoxOptions = new QGroupBox( this, "GroupBoxOptions" );
    GroupBoxOptions->setTitle( tr( "Ipkg options" ) );
    GroupBoxOptions->setColumnLayout(0, Qt::Vertical );
    GroupBoxOptions->layout()->setSpacing( 0 );
    GroupBoxOptions->layout()->setMargin( 0 );
    GroupBoxOptionsLayout = new QGridLayout( GroupBoxOptions->layout() );
    GroupBoxOptionsLayout->setAlignment( Qt::AlignTop );
    GroupBoxOptionsLayout->setSpacing( 2 );
    GroupBoxOptionsLayout->setMargin( 2 );

    _force_depends = new QCheckBox( GroupBoxOptions, "_force_depends" );
    QFont _force_depends_font(  _force_depends->font() );
    _force_depends_font.setPointSize( 8 );
    _force_depends->setFont( _force_depends_font ); 
    _force_depends->setText( tr( "-force-depends" ) );
    //_force_depends->setChecked( true );

    GroupBoxOptionsLayout->addWidget( _force_depends, 0, 0 );

    _force_reinstall = new QCheckBox( GroupBoxOptions, "_force_reinstall" );
    QFont _force_reinstall_font(  _force_reinstall->font() );
    _force_reinstall_font.setPointSize( 8 );
    _force_reinstall->setFont( _force_reinstall_font ); 
    _force_reinstall->setText( tr( "-force-reinstall" ) );

    GroupBoxOptionsLayout->addWidget( _force_reinstall, 1, 0 );

    _force_remove = new QCheckBox( GroupBoxOptions, "_force_remove" );
    QFont _force_remove_font(  _force_remove->font() );
    _force_remove_font.setPointSize( 8 );
    _force_remove->setFont( _force_remove_font ); 
    _force_remove->setText( tr( "-force-removal-of-essential-packages" ) );

    GroupBoxOptionsLayout->addWidget( _force_remove, 2, 0 );

    InstallDialogLayout->addWidget( GroupBoxOptions, 1, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
InstallDialog::~InstallDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool InstallDialog::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
    if ( ev->type() == QEvent::ApplicationFontChange ) {
	QFont _force_depends_font(  _force_depends->font() );
	_force_depends_font.setPointSize( 8 );
	_force_depends->setFont( _force_depends_font ); 
	QFont _force_reinstall_font(  _force_reinstall->font() );
	_force_reinstall_font.setPointSize( 8 );
	_force_reinstall->setFont( _force_reinstall_font ); 
	QFont _force_remove_font(  _force_remove->font() );
	_force_remove_font.setPointSize( 8 );
	_force_remove->setFont( _force_remove_font ); 
    }
    return ret;
}

