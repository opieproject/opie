/**********************************************************************
 ** Copyright (C) 2000 Trolltech AS.  All rights reserved.
 **
 ** This file is part of Qtopia Environment.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.trolltech.com/gpl/ for GPL licensing information.
 **
 ** Contact info@trolltech.com if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/
#include "security.h"

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/password.h>
#include <qpe/qpedialog.h>
#include <qpe/qcopenvelope_qws.h>

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qfile.h>
#include <qtextstream.h>

    Security::Security( QWidget* parent,  const char* name, WFlags fl )
: SecurityBase( parent, name, TRUE, WStyle_ContextHelp )
{
    valid=FALSE;
    Config cfg("Security");
    cfg.setGroup("Passcode");
    passcode = cfg.readEntry("passcode");
    passcode_poweron->setChecked(cfg.readBoolEntry("passcode_poweron",FALSE));
    cfg.setGroup("Sync");
    int auth_peer = cfg.readNumEntry("auth_peer",0xc0a88100);//new default 192.168.129.0/24
    int auth_peer_bits = cfg.readNumEntry("auth_peer_bits",24);
    selectNet(auth_peer,auth_peer_bits,TRUE);
  
    connect(syncnet, SIGNAL(textChanged(const QString&)),
            this, SLOT(setSyncNet(const QString&)));

    cfg.setGroup("Sync");
    QString sa = cfg.readEntry("syncapp","Qtopia");

    for (int i=0; i<syncapp->count(); i++) {
        if ( syncapp->text(i) == sa ) {
           syncapp->setCurrentItem(i);       	    
        }
    }  
    
    /*
       cfg.setGroup("Remote");
       if ( telnetAvailable() )
       telnet->setChecked(cfg.readEntry("allow_telnet"));
       else
       telnet->hide();

       if ( sshAvailable() )
       ssh->setChecked(cfg.readEntry("allow_ssh"));
       else
       ssh->hide();
       */

    QString configFile = QPEApplication::qpeDir() + "/etc/opie-login.conf";
    Config loginCfg(configFile,Config::File);
        
    loginCfg.setGroup("General");
    autoLoginName=loginCfg.readEntry("AutoLogin","");

    if (autoLoginName.stripWhiteSpace().isEmpty()) {
        autoLogin=false;
    } else {
        autoLogin=true;
    }

    cfg.setGroup("SyncMode");
    int mode = cfg.readNumEntry("Mode",2); // Default to Sharp
    syncModeCombo->setCurrentItem( mode - 1 );
    
    //since nobody knows what this is and it doesn't do anything, i'll hide it # CoreDump
    // is this work-in-progress or can it be removed?
    syncModeCombo->hide();

    connect(autologinToggle, SIGNAL(toggled(bool)), this, SLOT(toggleAutoLogin(bool)));
    connect(userlist, SIGNAL(activated(int)), this, SLOT(changeLoginName(int)));
    connect(changepasscode,SIGNAL(clicked()), this, SLOT(changePassCode()));
    connect(clearpasscode,SIGNAL(clicked()), this, SLOT(clearPassCode()));
    connect(syncapp,SIGNAL(activated(int)), this, SLOT(changeSyncApp()));
    connect(restoredefaults,SIGNAL(clicked()), this, SLOT(restoreDefaults()));    
    connect(deleteentry,SIGNAL(clicked()), this, SLOT(deleteListEntry()));
    
    loadUsers();
    updateGUI();

    dl = new QPEDialogListener(this);
    QPEApplication::showDialog( this );
}

Security::~Security()
{
}

void Security::deleteListEntry() 
{
    syncnet->removeItem(syncnet->currentItem());
}

void Security::restoreDefaults()
{		
    QMessageBox unrecbox(
    tr("Attention"),
    tr(	"<p>All user-defined net ranges will be lost."),
    QMessageBox::Warning,
    QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::NoButton,
    0, QString::null, TRUE, WStyle_StaysOnTop);
    unrecbox.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    unrecbox.setButtonText(QMessageBox::Yes, tr("Ok"));

    if ( unrecbox.exec() == QMessageBox::Yes)	
    {
        syncnet->clear();
        insertDefaultRanges();
    }	
}

void Security::insertDefaultRanges()
{    
    syncnet->insertItem( tr( "192.168.129.0/24" ) );
    syncnet->insertItem( tr( "192.168.1.0/24" ) );
    syncnet->insertItem( tr( "192.168.0.0/16" ) );
    syncnet->insertItem( tr( "172.16.0.0/12" ) );
    syncnet->insertItem( tr( "10.0.0.0/8" ) );
    syncnet->insertItem( tr( "1.0.0.0/8" ) );
    syncnet->insertItem( tr( "Any" ) );
    syncnet->insertItem( tr( "None" ) );
}

void Security::updateGUI()
{
    bool empty = passcode.isEmpty();

    changepasscode->setText( empty ? tr("Set passcode" )
            : tr("Change passcode" )  );
    passcode_poweron->setEnabled( !empty );
    clearpasscode->setEnabled( !empty );

    autologinToggle->setChecked(autoLogin);
    userlist->setEnabled(autoLogin);    
}


void Security::show()
{
    //valid=FALSE;
    setEnabled(FALSE);
    SecurityBase::show();
    if ( passcode.isEmpty() ) {
        // could insist...
        //changePassCode();
        //if ( passcode.isEmpty() )
        //reject();
    } else {
		if (!valid) // security passcode was not asked yet, so ask now
		{
			QString pc = enterPassCode(tr("Enter passcode"));
			if ( pc != passcode ) {
				QMessageBox::critical(this, tr("Passcode incorrect"),
						tr("The passcode entered is incorrect.\nAccess denied"));
				reject();
				return;
			}
		}
    }
    setEnabled(TRUE);
    valid=TRUE;
}

void Security::accept()
{
    applySecurity();
    QDialog::accept();
    QCopEnvelope env("QPE/System", "securityChanged()" );
}

void Security::done(int r)
{
    QDialog::done(r);
    close();
}

void Security::selectNet(int auth_peer,int auth_peer_bits, bool update)
{
    QString sn;
    if ( auth_peer_bits == 0 && auth_peer == 0 ) {
        sn = tr("Any");
    } else if ( auth_peer_bits == 32 && auth_peer == 0 ) {
        sn = tr("None");
    } else {
        sn =
            QString::number((auth_peer>>24)&0xff) + "."
            + QString::number((auth_peer>>16)&0xff) + "."
            + QString::number((auth_peer>>8)&0xff) + "."
            + QString::number((auth_peer>>0)&0xff) + "/"
            + QString::number(auth_peer_bits);
    }

    //insert user-defined list of netranges upon start
    if (update) {
	//User selected/active netrange first
	syncnet->insertItem( tr( sn ) );

	Config cfg("Security");
	cfg.setGroup("Sync");

	//set up defaults if needed, if someone manually deletes net0 he'll get a suprise hehe
	QString test = cfg.readEntry("net0","");
	if (test.isEmpty()) {
            insertDefaultRanges();
	} else {
            // 10 ought to be enough for everybody... :)
            // If you need more, don't forget to edit applySecurity() as well
            bool already_there=FALSE;
            for (int i=0; i<10; i++) {
                QString target, netrange;
                target.sprintf("net%d", i);
                netrange = cfg.readEntry(target,"");
                if (! netrange.isEmpty()){
                    //make sure we have no "twin" entries
                    for (int i=0; i<syncnet->count(); i++) {
                        if ( syncnet->text(i) == netrange ) {
                            already_there=TRUE;      	    
                        }
                    }
                    if (! already_there) {
                        syncnet->insertItem( tr( netrange ) );
                    } else {
                        already_there=FALSE;
                    }		
                }	
            }
        }	
    } 
     
    for (int i=0; i<syncnet->count(); i++) {
        if ( syncnet->text(i).left(sn.length()) == sn ) {
            syncnet->setCurrentItem(i);
            return;
        }
    }
    qDebug("No match for \"%s\"",sn.latin1());
}

void Security::parseNet(const QString& sn,int& auth_peer,int& auth_peer_bits)
{
    auth_peer=0;
    if ( sn == tr("Any") ) {
        auth_peer = 0;
        auth_peer_bits = 0;
    } else if ( sn == tr("None") ) {
        auth_peer = 0;
        auth_peer_bits = 32;
    } else {
        int x=0;
        for (int i=0; i<4; i++) {
            int nx = sn.find(QChar(i==3 ? '/' : '.'),x);
            auth_peer = (auth_peer<<8)|sn.mid(x,nx-x).toInt();
            x = nx+1;
        }
        uint n = (uint)sn.find(' ',x)-x;
        auth_peer_bits = sn.mid(x,n).toInt();
    }
}

void Security::loadUsers ( void )
{
    QFile passwd("/etc/passwd");
    if ( passwd.open(IO_ReadOnly) ) {
        QTextStream t( &passwd );
        QString s;
        QStringList account;
        while ( !t.eof() ) {
            account = QStringList::split(':',t.readLine());

            // Hide disabled accounts
            if (*account.at(1)!="*") {

                userlist->insertItem(*account.at(0));
                // Highlight this item if it is set to autologinToggle
                if ( *account.at(0) == autoLoginName)
                    userlist->setCurrentItem(userlist->count()-1);
            }
        }
        passwd.close();
    }

}

void Security::toggleAutoLogin(bool val)
{
    autoLogin=val;
    userlist->setEnabled(val);
    if (!autoLogin)
        autoLoginName=userlist->currentText();
}




void Security::setSyncNet(const QString& sn)
{
    int auth_peer,auth_peer_bits;
    parseNet(sn,auth_peer,auth_peer_bits);
    selectNet(auth_peer,auth_peer_bits,FALSE);
}

void Security::applySecurity()
{
    if ( valid ) {
        Config cfg("Security");
        cfg.setGroup("Passcode");
        cfg.writeEntry("passcode",passcode);
        cfg.writeEntry("passcode_poweron",passcode_poweron->isChecked());
        cfg.setGroup("Sync");
        int auth_peer=0;
        int auth_peer_bits;
        QString sn = syncnet->currentText();
        parseNet(sn,auth_peer,auth_peer_bits);
       
        //this is the *selected* (active) net range
        cfg.writeEntry("auth_peer",auth_peer);
        cfg.writeEntry("auth_peer_bits",auth_peer_bits);
	
	//write back all other net ranges in *cleartext*
	for (int i=0; i<10; i++) {
		QString target;
		target.sprintf("net%d", i);
        	cfg.writeEntry(target,syncnet->text(i));
	}
		
        cfg.writeEntry("syncapp",syncapp->currentText());
	
        /*
           cfg.setGroup("Remote");
           if ( telnetAvailable() )
           cfg.writeEntry("allow_telnet",telnet->isChecked());
           if ( sshAvailable() )
           cfg.writeEntry("allow_ssh",ssh->isChecked());
        // ### write ssh/telnet sys config files
        */

        QString configFile = QPEApplication::qpeDir() + "/etc/opie-login.conf";
        Config loginCfg(configFile,Config::File);
        loginCfg.setGroup("General");

        if (autoLogin) {
            loginCfg.writeEntry("AutoLogin",autoLoginName);
        } else {
            loginCfg.removeEntry("AutoLogin");
        }

    }
}
void Security::changeSyncApp()
{
    // Don't say i didn't tell ya
    if (syncapp->currentText() == "IntelliSync") {
	    QMessageBox attn(
		tr("WARNING"),
		tr("<p>Selecting IntelliSync here will disable the FTP password."
		    "<p>Every machine in your netrange will be able to sync with "
		    "your Zaurus!"),
		QMessageBox::Warning,
		QMessageBox::Cancel, QMessageBox::NoButton, QMessageBox::NoButton,
		0, QString::null, TRUE, WStyle_StaysOnTop);
	    attn.setButtonText(QMessageBox::Cancel, tr("Ok"));
	    attn.exec();
    } 
    updateGUI();
}



void Security::changeLoginName( int idx )
{
    autoLoginName = userlist->text(idx);;
    updateGUI();
}

void Security::changePassCode()
{
    QString new1;
    QString new2;

    do {
        new1 = enterPassCode(tr("Enter new passcode"));
        if ( new1.isNull() )
            return;
        new2 = enterPassCode(tr("Re-enter new passcode"));
        if ( new2.isNull() )
            return;
    } while (new1 != new2);

    passcode = new1;
    updateGUI();
}

void Security::clearPassCode()
{
    passcode = QString::null;
    updateGUI();
}


QString Security::enterPassCode(const QString& prompt)
{
    return Password::getPassword(prompt);
}

bool Security::telnetAvailable() const
{
    // ### not implemented
    return FALSE;
}

bool Security::sshAvailable() const
{
    // ### not implemented
    return FALSE;
}
