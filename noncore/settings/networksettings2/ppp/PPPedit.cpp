#include <qwidgetstack.h>

#include "PPPIPedit.h"
#include "PPPDNSedit.h"
#include "PPPAuthedit.h"
#include "PPPRunedit.h"
#include "PPPDialingedit.h"
#include "PPPedit.h"

PPPEdit::PPPEdit( QWidget * Parent ) : PPPGUI( Parent ){

    Auth = new PPPAuthEdit( Options_WS );
    IP = new PPPIPEdit( Options_WS );
    DNS = new PPPDNSEdit( Options_WS );
    Run = new PPPRunEdit( Options_WS );
    Dialing = new PPPDialingEdit( Options_WS );

    Options_WS->addWidget( Auth, 0 );
    Options_WS->addWidget( IP, 1 );
    Options_WS->addWidget( DNS, 2 );
    Options_WS->addWidget( Run, 3 );
    Options_WS->addWidget( Dialing, 4 );

    Options_WS->raiseWidget( 0 );
}

QString PPPEdit::acceptable( void ) {
    QString S;
    S = Auth->acceptable();
    if( S.isEmpty() ) {
      S = IP->acceptable();
      if( S.isEmpty() ) {
        S = DNS->acceptable();
      }
    }
    return S;
}

bool PPPEdit::commit( PPPData & Data ) {
    bool SM ;
    SM = Auth->commit( Data );
    SM |= IP->commit( Data );
    SM |= DNS->commit( Data );
    return SM;
}

void PPPEdit::showData( PPPData & Data ) {
    Auth->showData( Data ) ;
    IP->showData( Data );
    DNS->showData( Data );
}

void PPPEdit::SLOT_SelectTopic( int v ) {
    Options_WS->raiseWidget( v );
}
