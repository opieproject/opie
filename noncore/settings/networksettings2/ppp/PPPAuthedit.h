#include "pppdata.h"
#include "PPPAuthGUI.h"

class PPPAuthEdit  : public PPPAuthGUI {

public :

    PPPAuthEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData_t & Data );
    void showData( PPPData_t & Data );

private :

};
