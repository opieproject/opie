#include <qtimer.h>
#include <wextensions.h>
#include "wlandata.h"
#include "wlanGUI.h"

class ANetNodeInstance;
class AsDevice;
class WExtensions;

class WLanEdit  : public WLanGUI {

public :

    WLanEdit( QWidget * parent, ANetNodeInstance * NNI  );
    ~WLanEdit( void );
    QString acceptable( void );
    void showData( WLanData_t & Data );
    bool commit( WLanData_t & Data );

public slots :

    void SLOT_AutoRefresh( bool );
    void SLOT_Refresh( void );

private :

    ANetNodeInstance * NNI;
    AsDevice * Dev;
    QTimer RefreshTimer;
    WExtensions * WE;

};
