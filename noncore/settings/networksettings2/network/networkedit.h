#include "networkdata.h"
#include "networkGUI.h"

class NetworkEdit  : public NetworkGUI {

public :

    NetworkEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( NetworkData_t & Data );
    void showData( NetworkData_t & Data );

public slots :

    void SLOT_NetmaskModified( const QString & S );

private :

    bool updateList( QStringList &SL, QListBox * LB );
    void populateList( QStringList &SL, QListBox * LB );
};
