#include "pppdata.h"
#include "PPPGUI.h"

class PPPAuthEdit;
class PPPIPEdit;
class PPPDNSEdit;
class PPPRunEdit;
class PPPDialingEdit;

class PPPEdit  : public PPPGUI {

public :

    PPPEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData & Data );
    void showData( PPPData & Data );

public slots :

    void SLOT_SelectTopic( int );

private :

    PPPAuthEdit *     Auth;
    PPPDNSEdit *      DNS;
    PPPIPEdit *       IP;
    PPPRunEdit *      Run;
    PPPDialingEdit *  Dialing;

};
