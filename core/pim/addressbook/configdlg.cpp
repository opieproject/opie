#include "configdlg.h"
#include <qcheckbox.h>
#include <qradiobutton.h>

ConfigDlg::ConfigDlg( QWidget *parent = 0, const char *name = 0 ):
	ConfigDlg_Base(parent, name, true )
{}
    

bool ConfigDlg::useRegExp() const
{
	return m_useRegExp->isOn();
}
bool ConfigDlg::useWildCards() const
{
	return m_useWildCard->isOn();
}
bool ConfigDlg::useQtMail() const
{
	return m_useQtMail->isOn();
}
bool ConfigDlg::useOpieMail() const
{
	return m_useOpieMail->isOn();
}
bool ConfigDlg::beCaseSensitive() const
{
	return m_useCaseSensitive->isChecked();
}
bool ConfigDlg::signalWrapAround() const
{
	return m_signalWrapAround->isChecked();
}
void ConfigDlg::setUseRegExp( bool v )
{
	m_useRegExp->setChecked( v );
}
void ConfigDlg::setUseWildCards( bool v )
{
	m_useWildCard->setChecked( v );
}
void ConfigDlg::setBeCaseSensitive( bool v )
{
	m_useCaseSensitive->setChecked( v );
}
void ConfigDlg::setSignalWrapAround( bool v )
{
	m_signalWrapAround->setChecked( v );
}
void ConfigDlg::setQtMail( bool v )
{
	m_useQtMail->setChecked( v );
}
void ConfigDlg::setOpieMail( bool v )
{
	m_useOpieMail->setChecked( v );
}




