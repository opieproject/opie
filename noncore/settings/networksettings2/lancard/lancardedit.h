#include "lancarddata.h"
#include "lancardGUI.h"

class LanCardEdit  : public LanCardGUI {

public :

    LanCardEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( LanCardData & Data );
    void showData( LanCardData & Data );
    
private :

    bool ContainedObsoleteMAC;
};
