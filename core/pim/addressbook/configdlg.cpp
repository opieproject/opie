#include "configdlg.h"
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include <opie/ocontact.h>

#include "addresssettings.h"


ConfigDlg::ConfigDlg( QWidget *parent = 0, const char *name = 0 ):
	ConfigDlg_Base(parent, name, true )
{
	contFields = OContact::trfields();
	contFields.remove( tr("File As") );
	contFields.remove( tr("Gender") );

	for (uint i=0; i < contFields.count(); i++)
		allFieldListBox->insertItem( contFields[i] );
	connect ( m_addButton, SIGNAL( clicked() ), this, SLOT( slotItemAdd() ) );
	connect ( m_removeButton, SIGNAL( clicked() ), this, SLOT( slotItemRemove() ) );
	connect ( m_upButton, SIGNAL( clicked() ), this, SLOT( slotItemUp() ) );
	connect ( m_downButton, SIGNAL( clicked() ), this, SLOT( slotItemDown() ) );
}

void ConfigDlg::slotItemUp()
{
	qWarning( "void ConfigDlg::slotItemUp()" );
}

void ConfigDlg::slotItemDown()
{
	qWarning( "void ConfigDlg::slotItemDown()" );
}

void ConfigDlg::slotItemAdd()
{
	qWarning( "void ConfigDlg::slotItemAdd()" );
}

void ConfigDlg::slotItemRemove()
{
	qWarning( "void ConfigDlg::slotItemRemove()" );
}
    
void ConfigDlg::setConfig( const AbConfig& cnf )
{ 
	m_config = cnf; 

	m_useRegExp->setChecked( m_config.useRegExp() );
	m_useWildCard->setChecked( m_config.useWildCards() );
	m_useQtMail->setChecked( m_config.useQtMail() );
	m_useOpieMail->setChecked( m_config.useOpieMail() );
	m_useCaseSensitive->setChecked( m_config.beCaseSensitive() );

	switch( m_config.fontSize() ){
	case 0:
		m_smallFont->setChecked( true );
		m_normalFont->setChecked( false );
		m_largeFont->setChecked( false );
		break;
	case 1: 
		m_smallFont->setChecked( false );
		m_normalFont->setChecked( true );
		m_largeFont->setChecked( false );
		break;
	case 2: 
		m_smallFont->setChecked( false );
		m_normalFont->setChecked( false );
		m_largeFont->setChecked( true );
		break;
	}
	
}
    
AbConfig ConfigDlg::getConfig()
{ 
	m_config.setUseRegExp( m_useRegExp->isOn() );
	m_config.setUseWildCards( m_useWildCard->isOn() );
	m_config.setUseQtMail( m_useQtMail->isOn() );
	m_config.setUseOpieMail( m_useOpieMail->isOn() );
	m_config.setBeCaseSensitive( m_useCaseSensitive->isChecked() );

	if ( m_smallFont->isChecked() )
		m_config.setFontSize( 0 );
	if ( m_normalFont->isChecked() )
		m_config.setFontSize( 1 );
	if ( m_largeFont->isChecked() )
		m_config.setFontSize( 2 );

	return m_config;
}

