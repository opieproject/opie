#include "wlanimp.h"

/* Config class */
#include <qpe/config.h>
/* Global namespace */
#include <qpe/global.h>
/* system() */
#include <stdlib.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qpe/config.h>
#include <qtabwidget.h>
#include "interfacesetupimp.h"

WLANImp::WLANImp( QWidget* parent, const char* name, Interface *i, bool modal, WFlags fl):WLAN(parent, name, modal, fl){
  config = new Config("wireless");
  interfaceSetup = new InterfaceSetupImp(tabWidget, "InterfaceSetupImp", i);//, Qt::WDestructiveClose);
  //configure->setProfile(currentProfile);
  tabWidget->insertTab(interfaceSetup, "TCP/IP");

  readConfig();

}

WLANImp::~WLANImp( ){
  delete config;
}

void WLANImp::readConfig()
{
    qWarning( "WLANImp::readConfig() called." );
    config->setGroup( "Properties" );
    QString ssid = config->readEntry( "SSID", "any" );
    if( ssid == "any" || ssid == "ANY" ){
        essNon->setChecked( true );
    } else {
        essSpecific->setChecked( true );
        essSpecificLineEdit->setText( ssid );
    }
    QString mode = config->readEntry( "Mode", "Managed" );
    if( mode == "adhoc" ) {
        network802->setChecked( true );
    } else {
        networkInfrastructure->setChecked( true );
    }
    networkChannel->setValue( config->readNumEntry( "CHANNEL", 1 ) );
//    config->readEntry( "RATE", "auto" );
    config->readEntry( "dot11PrivacyInvoked" ) == "true" ? wepEnabled->setChecked( true ) : wepEnabled->setChecked( false );
    config->readEntry( "AuthType", "opensystem" );
    config->readEntry( "PRIV_KEY128", "false" ) == "false" ? key40->setChecked( true ) : key128->setChecked( true );
    int defaultkey = config->readNumEntry( "dot11WEPDefaultKeyID", 0 );
    switch( defaultkey ){
    case 0:
        keyRadio0->setChecked( true );
	break;
    case 1:
        keyRadio1->setChecked( true );
	break;
    case 2:
        keyRadio2->setChecked( true );
	break;
    case 3:
        keyRadio3->setChecked( true );
	break;
    }
    keyLineEdit0->setText(config->readEntry( "dot11WEPDefaultKey0" ));
    keyLineEdit1->setText(config->readEntry( "dot11WEPDefaultKey1" ));
    keyLineEdit2->setText(config->readEntry( "dot11WEPDefaultKey2" ));
    keyLineEdit3->setText(config->readEntry( "dot11WEPDefaultKey3" ));
    return;
}

bool WLANImp::writeConfig()
{
    qWarning( "WLANImp::writeConfig() called." );
    config->setGroup( "Properties" );
    if( essNon->isChecked() ) {
        config->writeEntry( "SSID", "any" );
    } else {
        config->writeEntry( "SSID", essSpecificLineEdit->text() );
    }
    if( networkInfrastructure->isChecked() ){
        config->writeEntry( "Mode", "Managed" );
    } else if( network802->isChecked() ){
        config->writeEntry( "Mode", "adhoc" );
    }
    config->writeEntry( "CHANNEL", networkChannel->value() );
//    config->readEntry( "RATE", "auto" );
    wepEnabled->isChecked() ? config->writeEntry( "dot11PrivacyInvoked", "true" ) : config->writeEntry( "dot11PrivacyInvoked", "false" );
    authOpen->isChecked() ? config->writeEntry( "AuthType", "opensystem" ) : config->writeEntry( "AuthType", "sharedkey" );
    key40->isChecked() ? config->writeEntry( "PRIV_KEY128", "false" ) : config->writeEntry( "PRIV_KEY128", "true" );
    if( keyRadio0->isChecked() ){
        config->writeEntry( "dot11WEPDefaultKeyID", 0 );
    } else if( keyRadio1->isChecked() ){
        config->writeEntry( "dot11WEPDefaultKeyID", 1 );
    } else if( keyRadio2->isChecked() ){
        config->writeEntry( "dot11WEPDefaultKeyID", 2 );
    } else if( keyRadio3->isChecked() ){
        config->writeEntry( "dot11WEPDefaultKeyID", 3 );
    }
    config->writeEntry( "dot11WEPDefaultKey0", keyLineEdit0->text() );
    config->writeEntry( "dot11WEPDefaultKey1", keyLineEdit1->text() );
    config->writeEntry( "dot11WEPDefaultKey2", keyLineEdit2->text() );
    config->writeEntry( "dot11WEPDefaultKey3", keyLineEdit3->text() );
    return true;
    return writeWirelessOpts( );
}

/**
 */
void WLANImp::accept()
{
  if ( writeConfig() ){
    interfaceSetup->saveChanges();
    QDialog::accept();
  }
}

bool WLANImp::writeWirelessOpts( QString scheme )
{
    qWarning( "WLANImp::writeWirelessOpts entered." );
    QString prev = "/etc/pcmcia/wireless.opts";
    QFile prevFile(prev);
    if ( !prevFile.open( IO_ReadOnly ) )
	return false;

    QString tmp = "/etc/pcmcia/wireless.opts-qpe-new";
    QFile tmpFile(tmp);
    if ( !tmpFile.open( IO_WriteOnly ) )
	return false;

    bool retval = true;
    
    QTextStream in( &prevFile );
    QTextStream out( &tmpFile );

    config->setGroup("Properties");
    
    QString line;
    bool found=false;
    bool done=false;
    while ( !in.atEnd() ) {
	QString line = in.readLine();
	QString wline = line.simplifyWhiteSpace();
	if ( !done ) {
	    if ( found ) {
		// skip existing entry for this scheme, and write our own.
		if ( wline == ";;" ) {
		    found = false;
		    continue;
		} else {
		    continue;
		}
	    } else {
		if ( wline.left(scheme.length()+7) == scheme + ",*,*,*)" ) {
		    found=true;
		    continue; // skip this line
		} else if ( wline == "esac" || wline == "*,*,*,*)" ) {
		    // end - add new entry
		    // Not all fields have a GUI, but all are supported
		    // in the letwork configuration files.
		    static const char* txtfields[] = {
			0
		    };
		    QString readmode = config->readEntry( "Mode", "Managed" );
		    QString mode;
		    if( readmode == "Managed" ){
			mode = readmode;
		    } else if( readmode == "adhoc" ){
			mode = "Ad-Hoc";
		    }
		    QString key;
		    if( wepEnabled->isChecked() ){
			int defaultkey = config->readNumEntry( "dot11WEPDefaultKeyID", 0 );
			switch( defaultkey ){
			    case 0:
				key += keyLineEdit0->text();
				break;
			    case 1:
				key += keyLineEdit1->text();
				break;
			    case 2:
				key += keyLineEdit2->text();
				break;
			    case 3:
				key += keyLineEdit3->text();
				break;
			}
		        if( config->readEntry( "AuthType", "opensystem" ) == "opensystem")
			    key += " open";
		    }
		    out << scheme << ",*,*,*)" << "\n"
			<< "    ESSID=" << Global::shellQuote( config->readEntry( "SSID", "any" ) ) << "\n"
			<< "    MODE=" << mode << "\n"
			<< "    KEY=" << Global::shellQuote( key ) << "\n"
			<< "    RATE=" << "auto" << "\n"
			;
			if( mode != "Managed"  )
				out << "    CHANNEL=" << config->readNumEntry( "CHANNEL", 1 ) << "\n";
		    const char** f = txtfields;
		    while (*f) {
			out << "    " << *f << "=" << config->readEntry(*f,"") << "\n";
			++f;
		    }
		    out << "    ;;\n";
		    done = true;
		}
	    }
	}
	out << line << "\n";
    }

    prevFile.close();
    tmpFile.close();
    QString initpath;
    //system("cardctl suspend");
    if( QDir("/etc/rc.d/init.d").exists() ){
	initpath = "/etc/rc.d/init.d";
    } else if( QDir("/etc/init.d").exists() ){
	initpath = "/etc/init.d";
    }
    if( initpath )
	    system(QString("%1/pcmcia stop").arg(initpath));

    if( system( "mv " + tmp + " " + prev ) )
	retval = false;
//#ifdef USE_SCHEMES
//    if ( retval )
//	SchemeChanger::changeScheme(scheme);
//#endif

    //system("cardctl resume");
    if( initpath )
	    system(QString("%1/pcmcia start").arg(initpath));

    return retval;
}
