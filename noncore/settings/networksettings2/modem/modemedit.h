#include "modemdata.h"
#include "modemGUI.h"

class ModemEdit  : public ModemGUI {

public :

    ModemEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( ModemData & Data );
    void showData( ModemData & Data );
};
