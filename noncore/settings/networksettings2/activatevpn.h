#include "activatevpnGUI.h"
#include "nsdata.h"

class ActivateVPN : public ActivateVPNGUI {

      Q_OBJECT

public :

      ActivateVPN( void );
      ~ActivateVPN( void );

public slots :

      void SLOT_ChangedVPNSetting( QListViewItem * );

private :

      NetworkSettingsData NSD;
};
