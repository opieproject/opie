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

WLANImp::WLANImp( Config &cfg, QWidget* parent, const char* name):WLAN(parent, name),config(cfg){
  readConfig();
}

void WLANImp::readConfig()
{
    qWarning( "WLANImp::readConfig() called." );
    config.setGroup( "Properties" );
    QString ssid = config.readEntry( "SSID", "any" );
    if( ssid == "any" || ssid == "ANY" ){
        essNon->setChecked( TRUE );
    } else {
        essSpecific->setChecked( TRUE );
        essSpecificLineEdit->setText( ssid );
    }
    QString mode = config.readEntry( "Mode", "Managed" );
    if( mode == "adhoc" ) {
        network802->setChecked( TRUE );
    } else {
        networkInfrastructure->setChecked( TRUE );
    }
    networkChannel->setValue( config.readNumEntry( "CHANNEL", 1 ) );
//    config.readEntry( "RATE", "auto" );
    config.readEntry( "dot11PrivacyInvoked" ) == "true" ? wepEnabled->setChecked( TRUE ) : wepEnabled->setChecked( FALSE );
    config.readEntry( "AuthType", "opensystem" );
    config.readEntry( "PRIV_KEY128", "false" ) == "false" ? key40->setChecked( TRUE ) : key128->setChecked( TRUE );
    int defaultkey = config.readNumEntry( "dot11WEPDefaultKeyID", 0 );
    switch( defaultkey ){
    case 0:
        keyRadio0->setChecked( TRUE );
	break;
    case 1:
        keyRadio1->setChecked( TRUE );
	break;
    case 2:
        keyRadio2->setChecked( TRUE );
	break;
    case 3:
        keyRadio3->setChecked( TRUE );
	break;
    }
    keyLineEdit0->setText(config.readEntry( "dot11WEPDefaultKey0" ));
    keyLineEdit1->setText(config.readEntry( "dot11WEPDefaultKey1" ));
    keyLineEdit2->setText(config.readEntry( "dot11WEPDefaultKey2" ));
    keyLineEdit3->setText(config.readEntry( "dot11WEPDefaultKey3" ));
    return;
}

bool WLANImp::writeConfig()
{
    qWarning( "WLANImp::writeConfig() called." );
    config.setGroup( "Properties" );
    if( essNon->isChecked() ) {
        config.writeEntry( "SSID", "any" );
    } else {
        config.writeEntry( "SSID", essSpecificLineEdit->text() );
    }
    if( networkInfrastructure->isChecked() ){
        config.writeEntry( "Mode", "Managed" );
    } else if( network802->isChecked() ){
        config.writeEntry( "Mode", "adhoc" );
    }
    config.writeEntry( "CHANNEL", networkChannel->value() );
//    config.readEntry( "RATE", "auto" );
    wepEnabled->isChecked() ? config.writeEntry( "dot11PrivacyInvoked", "true" ) : config.writeEntry( "dot11PrivacyInvoked", "false" );
    authOpen->isChecked() ? config.writeEntry( "AuthType", "opensystem" ) : config.writeEntry( "AuthType", "sharedkey" );
    key40->isChecked() ? config.writeEntry( "PRIV_KEY128", "false" ) : config.writeEntry( "PRIV_KEY128", "true" );
    if( keyRadio0->isChecked() ){
        config.writeEntry( "dot11WEPDefaultKeyID", 0 );
    } else if( keyRadio1->isChecked() ){
        config.writeEntry( "dot11WEPDefaultKeyID", 1 );
    } else if( keyRadio2->isChecked() ){
        config.writeEntry( "dot11WEPDefaultKeyID", 2 );
    } else if( keyRadio3->isChecked() ){
        config.writeEntry( "dot11WEPDefaultKeyID", 3 );
    }
    config.writeEntry( "dot11WEPDefaultKey0", keyLineEdit0->text() );
    config.writeEntry( "dot11WEPDefaultKey1", keyLineEdit1->text() );
    config.writeEntry( "dot11WEPDefaultKey2", keyLineEdit2->text() );
    config.writeEntry( "dot11WEPDefaultKey3", keyLineEdit3->text() );
    return writeWirelessOpts( config );
//    return TRUE;
}

/**
 */
void WLANImp::accept()
{
  if ( writeConfig() )
    QDialog::accept();
}

void WLANImp::done ( int r )
{
	QDialog::done ( r );
	close ( );
}

bool WLANImp::writeWirelessOpts( Config &config, QString scheme )
{
    qWarning( "WLANImp::writeWirelessOpts entered." );
    QString prev = "/etc/pcmcia/wireless.opts";
    QFile prevFile(prev);
    if ( !prevFile.open( IO_ReadOnly ) )
	return FALSE;

    QString tmp = "/etc/pcmcia/wireless.opts-qpe-new";
    QFile tmpFile(tmp);
    if ( !tmpFile.open( IO_WriteOnly ) )
	return FALSE;

    bool retval = TRUE;
    
    QTextStream in( &prevFile );
    QTextStream out( &tmpFile );

    config.setGroup("Properties");
    
    QString line;
    bool found=FALSE;
    bool done=FALSE;
    while ( !in.atEnd() ) {
	QString line = in.readLine();
	QString wline = line.simplifyWhiteSpace();
	if ( !done ) {
	    if ( found ) {
		// skip existing entry for this scheme, and write our own.
		if ( wline == ";;" ) {
		    found = FALSE;
		    continue;
		} else {
		    continue;
		}
	    } else {
		if ( wline.left(scheme.length()+7) == scheme + ",*,*,*)" ) {
		    found=TRUE;
		    continue; // skip this line
		} else if ( wline == "esac" || wline == "*,*,*,*)" ) {
		    // end - add new entry
		    // Not all fields have a GUI, but all are supported
		    // in the letwork configuration files.
		    static const char* txtfields[] = {
			0
		    };
		    QString readmode = config.readEntry( "Mode", "Managed" );
		    QString mode;
		    if( readmode == "Managed" ){
			mode = readmode;
		    } else if( readmode == "adhoc" ){
			mode = "Ad-Hoc";
		    }
		    QString key;
		    if( wepEnabled->isChecked() ){
			int defaultkey = config.readNumEntry( "dot11WEPDefaultKeyID", 0 );
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
		        if( config.readEntry( "AuthType", "opensystem" ) == "opensystem")
			    key += " open";
		    }
		    out << scheme << ",*,*,*)" << "\n"
			<< "    ESSID=" << Global::shellQuote( config.readEntry( "SSID", "any" ) ) << "\n"
			<< "    MODE=" << mode << "\n"
			<< "    KEY=" << Global::shellQuote( key ) << "\n"
			<< "    RATE=" << "auto" << "\n"
			;
			if( mode != "Managed"  )
				out << "    CHANNEL=" << config.readNumEntry( "CHANNEL", 1 ) << "\n";
		    const char** f = txtfields;
		    while (*f) {
			out << "    " << *f << "=" << config.readEntry(*f,"") << "\n";
			++f;
		    }
		    out << "    ;;\n";
		    done = TRUE;
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
	retval = FALSE;
//#ifdef USE_SCHEMES
//    if ( retval )
//	SchemeChanger::changeScheme(scheme);
//#endif

    //system("cardctl resume");
    if( initpath )
	    system(QString("%1/pcmcia start").arg(initpath));

    return retval;
}
