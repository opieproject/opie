#include "activateprofileGUI.h"
#include "nsdata.h"

class ActivateProfile : public ActivateProfileGUI {

      Q_OBJECT

public :

      ActivateProfile( const char * interface );
      ~ActivateProfile( void );

      long selectedProfile( void );

private :

      NetworkSettingsData NSD;
      QList<NodeCollection> Possible;
};
