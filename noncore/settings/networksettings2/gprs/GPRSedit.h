#include "GPRSdata.h"
#include "GPRSGUI.h"

class GPRSEdit  : public GPRSGUI {

public :

    GPRSEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( GPRSData & Data );
    void showData( GPRSData & Data );

public slots :

    void SLOT_AddRoute();
    void SLOT_DeleteRoute();

private :

};
