#include "activatevpnGUI.h"
#include "nsdata.h"

class ActivateVPN : public ActivateVPNGUI {

      Q_OBJECT

public :

      ActivateVPN( const QString & Interface );
      ~ActivateVPN( void );

public slots :

      void SLOT_ChangedVPNSetting( QListViewItem * );

private :

      NetworkSettingsData NSD;
};
