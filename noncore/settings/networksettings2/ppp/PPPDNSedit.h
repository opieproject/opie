#include "pppdata.h"
#include "PPPDNSGUI.h"

class PPPDNSEdit  : public PPPDNSGUI {

public :

    PPPDNSEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData_t & Data );
    void showData( PPPData_t & Data );

public slots :

    void SLOT_AddServer( void );
    void SLOT_RemoveServer( void );

private :

};
