#include "usbdata.h"
#include "usbGUI.h"

class USBEdit  : public USBGUI {

public :

    USBEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( USBData & Data );
    void showData( USBData & Data );
};
