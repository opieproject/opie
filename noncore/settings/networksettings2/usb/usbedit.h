#include "usbdata.h"
#include "usbGUI.h"

class USBEdit  : public USBGUI {

public :

    USBEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( USBData_t & Data );
    void showData( USBData_t & Data );
};
