#include <qwidgetstack.h>

#include "PPPIPedit.h"
#include "PPPDNSedit.h"
#include "PPPAuthedit.h"
#include "PPPedit.h"

PPPEdit::PPPEdit( QWidget * Parent ) : PPPGUI( Parent ){

    Auth = new PPPAuthEdit( Options_WS );
    IP = new PPPIPEdit( Options_WS );
    DNS = new PPPDNSEdit( Options_WS );
    Options_WS->addWidget( Auth, 0 );
    Options_WS->addWidget( IP, 1 );
    Options_WS->addWidget( DNS, 2 );

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

bool PPPEdit::commit( PPPData_t & Data ) {
    bool SM ;
    SM = Auth->commit( Data );
    SM |= IP->commit( Data );
    SM |= DNS->commit( Data );
    return SM;
}

void PPPEdit::showData( PPPData_t & Data ) {
    Auth->showData( Data ) ;
    IP->showData( Data );
    DNS->showData( Data );
}

void PPPEdit::SLOT_SelectTopic( int v ) {
    Options_WS->raiseWidget( v );
}
