#include <qtimer.h>
#include <wextensions.h>
#include "wlandata.h"
#include "wlanGUI.h"

class ANetNodeInstance;
class RuntimeInfo;
class WExtensions;

class WLanEdit  : public WLanGUI {

public :

    WLanEdit( QWidget * parent, ANetNodeInstance * NNI  );
    ~WLanEdit( void );
    QString acceptable( void );
    void showData( WLanData & Data );
    bool commit( WLanData & Data );

public slots :

    void SLOT_AutoRefresh( bool );
    void SLOT_Refresh( void );

private :

    ANetNodeInstance * NNI;
    RuntimeInfo * Dev;
    QTimer RefreshTimer;
    WExtensions * WE;

};
