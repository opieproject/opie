#include "pppdata.h"
#include "PPPRunGUI.h"

class PPPRunEdit  : public PPPRunGUI {

public :

    PPPRunEdit( QWidget * parent );

    QString acceptable( void );
    bool commit( PPPData & Data );
    void showData( PPPData & Data );

private :

};
