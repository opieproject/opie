#include "nsdata.h"
#include "networksettingsGUI.h"
#include "resources.h"

class ANetNode;
class ANetNodeInstance;
class QTimer;
class QListBoxItem;

class NetworkSettings : public NetworkSettingsGUI {

      Q_OBJECT

public :

      NetworkSettings( QWidget *parent=0, 
                       const char *name=0, 
                       WFlags fl = 0 );
      ~NetworkSettings( void );

      static QString appName( void ) 
        { return QString::fromLatin1("networksettings"); }

      bool isModified( void ) 
        { return NSD.isModified(); }
      void setModified( bool m ) 
        { NSD.setModified( m ); }

public slots :

      void SLOT_AddNode( void );
      void SLOT_DeleteNode( void );
      void SLOT_ShowNode( QListBoxItem * );
      void SLOT_EditNode( QListBoxItem * );
      void SLOT_CheckState( void );
      void SLOT_Enable( void );
      void SLOT_On( void );
      void SLOT_Connect( void );
      void SLOT_GenerateConfig( void );
      void SLOT_RefreshStates( void );
      void SLOT_QCopMessage( const QCString&,const QByteArray& );

private :

      void updateProfileState( QListBoxItem * it );
      QTimer * UpdateTimer;
      NetworkSettingsData NSD;

};
