#include "pppdata.h"
#include "PPPIPGUI.h"

class PPPIPEdit  : public PPPIPGUI {

public :

    PPPIPEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData & Data );
    void showData( PPPData & Data );

private :

};
