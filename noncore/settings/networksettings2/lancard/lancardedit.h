#include "lancarddata.h"
#include "lancardGUI.h"

class ALanCard;

class LanCardEdit  : public LanCardGUI {

public :

    LanCardEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( LanCardData & Data );
    void showData( ALanCard * NNI );

public slots :

    void SLOT_ScanCards( void );
    void SLOT_RemoveUnknown( void );
    
private :

    void populateList( void );
    ALanCard * NNI;
};
