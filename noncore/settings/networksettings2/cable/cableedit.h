#include "cabledata.h"
#include "cableGUI.h"

class CableEdit  : public CableGUI {

public :

    CableEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( CableData & D );
    void showData( CableData & D );
};
