#include "profiledata.h"
#include "profileGUI.h"

#include <qtimer.h>
class ANetNodeInstance;
class AsDevice;

class ProfileEdit  : public ProfileGUI {

    Q_OBJECT

public :

    ProfileEdit( QWidget * parent, ANetNodeInstance * NNI );
    QString acceptable( void );
    bool commit( ProfileData_t & Data );
    void showData( ProfileData_t & Data );

public slots :

    void SLOT_AutoRefresh( bool );
    void SLOT_Refresh( void );

private :

    QTimer RefreshTimer;
    ANetNodeInstance * NNI;
    AsDevice * Dev;
};
