#include "pppdata.h"
#include "PPPDialingGUI.h"

class PPPDialingEdit  : public PPPDialingGUI {

public :

    PPPDialingEdit( QWidget * parent );

    QString acceptable( void );
    bool commit( PPPData & Data );
    void showData( PPPData & Data );

private :

};
