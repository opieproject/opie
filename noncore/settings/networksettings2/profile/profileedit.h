#include "profiledata.h"
#include "profileGUI.h"

#include <qtimer.h>
class ANetNodeInstance;
class RuntimeInfo;

class ProfileEdit  : public ProfileGUI {

    Q_OBJECT

public :

    ProfileEdit( QWidget * parent, ANetNodeInstance * NNI );
    QString acceptable( void );
    bool commit( ProfileData & Data );
    void showData( ProfileData & Data );

public slots :

    void SLOT_AutoRefresh( bool );
    void SLOT_Refresh( void );

private :

    QTimer RefreshTimer;
    ANetNodeInstance * NNI;
    RuntimeInfo * Dev;
};
