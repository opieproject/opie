#include "irdadata.h"
#include "irdaGUI.h"

class IRDAEdit  : public IRDAGUI {

public :

    IRDAEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( IRDAData & Data );
    void showData( IRDAData & Data );
};
