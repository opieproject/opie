#include "editconnectionGUI.h"

#include <qptrdict.h>
#include <qvector.h>
#include "netnode.h"

class ANetNodeInstance;
class ANetNode;
class QListViewItem;
class NetNodeLVI;
class QCloseEvent;

class EditConnection : public EditConnectionGUI {

      Q_OBJECT

public :

      EditConnection( QWidget * P );

      ANetNodeInstance * netNodeInstance( void );
      void setConnection( NodeCollection * NC );
      NodeCollection * connection( void );

public slots :

      void SLOT_AutoCollapse( bool );
      void SLOT_SelectNode( QListViewItem * it );
      void SLOT_AlterTab( const QString & S );
      virtual void accept( void );

private :

      NodeCollection * getTmpCollection( void );
      void updateGUI( QListViewItem * it, ANetNode * NN );
      void buildFullTree( void );
      void buildSubTree( QListViewItem * Parent, ANetNode * NN );
      void disableTree( QListViewItem * it, bool Mode);
      void enablePath( QListViewItem * it, bool pha);

      bool haveCompleteConfig( QListViewItem * it );
      QPtrDict<ANetNode> * Mapping;
      NodeCollection * SelectedNodes;
      bool           TmpIsValid;
      NodeCollection TmpCollection;

      static bool AutoCollapse;

};
