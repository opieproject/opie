#include "wlandata.h"
#include "wlanGUI.h"

class WLanEdit  : public WLanGUI {

public :

    WLanEdit( QWidget * parent );
    QString acceptable( void );
    void showData( WLanData_t & Data );
    bool commit( WLanData_t & Data );
};
