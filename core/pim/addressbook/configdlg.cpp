#include "configdlg.h"
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qlistbox.h>

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
}
    
void ConfigDlg::setConfig( const AbConfig& cnf )
{ 
	m_config = cnf; 

	m_useRegExp->setChecked( m_config.useRegExp() );
	m_useWildCard->setChecked( m_config.useWildCards() );
	m_useQtMail->setChecked( m_config.useQtMail() );
	m_useOpieMail->setChecked( m_config.useOpieMail() );
	m_useCaseSensitive->setChecked( m_config.beCaseSensitive() );
	
}
    
AbConfig ConfigDlg::getConfig()
{ 
	m_config.setUseRegExp( m_useRegExp->isOn() );
	m_config.setUseWildCards( m_useWildCard->isOn() );
	m_config.setUseQtMail( m_useQtMail->isOn() );
	m_config.setUseOpieMail( m_useOpieMail->isOn() );
	m_config.setBeCaseSensitive( m_useCaseSensitive->isChecked() );

	return m_config;
}

