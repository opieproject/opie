#include "nsdata.h"
#include "networksettingsGUI.h"
#include "resources.h"

class ANetNode;
class ANetNodeInstance;
class QTimer;
class QListBoxItem;
class QEvent;
class OLedBox;

class NetworkSettings : public NetworkSettingsGUI {

      Q_OBJECT

public :

      NetworkSettings( QWidget *parent=0, 
                       const char *name=0, 
                       WFlags fl = 0 );
      virtual ~NetworkSettings( void );

      static QString appName( void ) 
        { return QString::fromLatin1("networksettings"); }

public slots :

      void SLOT_AddNode( void );
      void SLOT_DeleteNode( void );
      void SLOT_ShowNode( QListBoxItem * );
      void SLOT_EditNode( QListBoxItem * );
      void SLOT_CheckState( void );

      void SLOT_Up( void );
      void SLOT_Down( void );
      void SLOT_Disable( bool );

      void SLOT_GenerateConfig( void );
      void SLOT_RefreshStates( void );
      void SLOT_QCopMessage( const QCString&,const QByteArray& );
      void SLOT_ToProfile( void );
      void SLOT_ToMessages( void );
      void SLOT_CmdMessage( const QString & S );

private :

      void updateProfileState( QListBoxItem * it );
      QTimer * UpdateTimer;
      NetworkSettingsData NSD;
      /*

          no leds : not present, unknown, unchecked or disabled
          (1) down : hardware present but inactive
          (2) available : hardware present and active
          (3) up : present active and connected

      */
      OLedBox * Leds[3];
};
