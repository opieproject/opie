#include "networksettingsGUI.h"
#include "resources.h"

class ANetNode;
class ANetNodeInstance;
class QTimer;
class QIconViewItem;

class NetworkSettingsData {

public :

      NetworkSettingsData( void );
      ~NetworkSettingsData( void );

      void loadSettings( void );
      QString saveSettings( void );

      QString generateSettings( bool Force = FALSE );

      bool isModified( void ) 
        { return IsModified; }
      void setModified( bool m ) 
        { IsModified = m; }

      void forceGeneration( bool m ) 
        { Force = m; }

private :

      bool IsModified;
      bool Force;

};

class NetworkSettings : public NetworkSettingsGUI {

      Q_OBJECT

public :

      NetworkSettings( QWidget *parent=0, 
                       const char *name=0, 
                       WFlags fl = 0 );
      ~NetworkSettings( void );

      static QString appName( void ) 
        { return QString::fromLatin1("networksettings"); }

      static void canStart( const char * Interface );
      static bool regenerate( void );

      bool isModified( void ) 
        { return NSD.isModified(); }
      void setModified( bool m ) 
        { NSD.setModified( m ); }

public slots :

      void SLOT_AddNode( void );
      void SLOT_DeleteNode( void );
      void SLOT_ShowNode( QIconViewItem * );
      void SLOT_EditNode( QIconViewItem * );
      void SLOT_CheckState( void );
      void SLOT_Enable( void );
      void SLOT_On( void );
      void SLOT_Connect( void );
      void SLOT_GenerateConfig( void );
      void SLOT_RefreshStates( void );

private :

      void updateProfileState( QIconViewItem * it );
      QTimer * UpdateTimer;
      NetworkSettingsData NSD;

};
