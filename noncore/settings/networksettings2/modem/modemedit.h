#include "modemdata.h"
#include "modemGUI.h"

class ModemEdit  : public ModemGUI {

public :

    ModemEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( ModemData_t & Data );
    void showData( ModemData_t & Data );
};
