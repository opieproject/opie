#include "networkdata.h"
#include "networkGUI.h"

class QListBox;

class NetworkEdit  : public NetworkGUI {

public :

    NetworkEdit( QWidget * parent );
    QString acceptable( void );
    bool commit( NetworkData_t & Data );
    void showData( NetworkData_t & Data );

public slots :

    void SLOT_NetmaskModified( const QString & S );
    void SLOT_ShowCommand( QListBoxItem * LBI );
    void SLOT_Add( void );
    void SLOT_Remove( void );
    void SLOT_Up( void );
    void SLOT_Down( void );

private :

    QListBox * getActiveLB( void );
    bool updateList( QStringList &SL, QListBox * LB );
    void populateList( QStringList &SL, QListBox * LB );
};
