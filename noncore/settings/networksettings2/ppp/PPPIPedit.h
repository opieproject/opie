#include "pppdata.h"
#include "PPPIPGUI.h"

class PPPIPEdit  : public PPPIPGUI {

public :

    PPPIPEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData_t & Data );
    void showData( PPPData_t & Data );

private :

};
