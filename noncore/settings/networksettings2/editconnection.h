#include "editconnectionGUI.h"

#include <qptrdict.h>
#include <qvector.h>
#include "netnode.h"

class ANetNodeInstance;
class ANetNode;
class QListViewItem;
class NetNodeLVI;
class QCloseEvent;

class EditNetworkSetup : public EditNetworkSetupGUI {

      Q_OBJECT

public :

      EditNetworkSetup( QWidget * P );

      ANetNodeInstance * netNodeInstance( void );
      void setNetworkSetup( NetworkSetup * NC );
      NetworkSetup * networkSetup( void );

public slots :

      void SLOT_AutoCollapse( bool );
      void SLOT_SelectNode( QListViewItem * it );
      void SLOT_AlterTab( const QString & S );
      virtual void accept( void );

private :

      NetworkSetup * getTmpCollection( void );
      void updateGUI( QListViewItem * it, ANetNode * NN );
      void buildFullTree( void );
      void buildSubTree( QListViewItem * Parent, ANetNode * NN );
      void disableTree( QListViewItem * it, bool Mode);
      void enablePath( QListViewItem * it, bool pha);

      bool haveCompleteConfig( QListViewItem * it );
      QPtrDict<ANetNode> * Mapping;
      NetworkSetup * SelectedNodes;
      bool           TmpIsValid;
      NetworkSetup TmpCollection;

      static bool AutoCollapse;

};
