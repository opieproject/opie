#include "pppdata.h"
#include "PPPAuthGUI.h"

class PPPAuthEdit  : public PPPAuthGUI {

public :

    PPPAuthEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( PPPData & Data );
    void showData( PPPData & Data );
    bool PAP_Checked( void );


private :

};
