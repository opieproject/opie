#include "pppimp.h"

PPPConfigureImp::PPPConfigureImp( QWidget* parent, const char* name, bool modal, WFlags fl) : PPP(parent, name, modal, fl) {

}

/**
 * Open up the peers file and read any set settings
 * @param peer the peer file to edit.
 */ 
void PPPConfigureImp::setPeer(const QString &newPeer){
  peer = newPeer;
}

/**
 * Save the settings for the current peer.
 */ 
void PPPConfigureImp::accept(){
  // Check to make sure that all of the inputs are valid.

  // Save settings.
/**
/dev/modem or just modem
<speed> any
defaultroute - automaticly add route
noipdefault - get an ip from the server
usepeerdns - automaticly add dns
crtscts - flow control enabled
name <name> - user name (and do secret file)
*/ 
	
  // Make sure it is set in interfaces so pon/off can be used.

  // Close out the dialog
  QDialog::accept();
}
		
// pppconfigureimp.h

