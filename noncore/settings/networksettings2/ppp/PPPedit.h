#include "pppdata.h"
#include "PPPGUI.h"

class PPPAuthEdit;
class PPPIPEdit;
class PPPDNSEdit;

class PPPEdit  : public PPPGUI {

public :

    PPPEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData_t & Data );
    void showData( PPPData_t & Data );

public slots :

    void SLOT_SelectTopic( int );

private :

    PPPAuthEdit * Auth;
    PPPDNSEdit *  DNS;
    PPPIPEdit *   IP;

};
