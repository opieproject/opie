#include <qlistbox.h>
#include <qlabel.h>
#include "activateprofile.h"

ActivateProfile::ActivateProfile( const char * Interface ) :
        ActivateProfileGUI( 0, 0, TRUE ), NSD() {

    Possible = NSD.collectPossible( Interface );

    DeviceName_LBL->setText( Interface );
    Profiles_LB->clear();
    for( NodeCollection * NC = Possible.first();
         NC;
         NC = Possible.next() ) {
      Profiles_LB->insertItem( NC->devicePixmap(), 
                               NC->name() );
    }
}

ActivateProfile::~ActivateProfile( void ) {
}

long ActivateProfile::selectedProfile( void ) {
    for( unsigned int i = 0 ; i < Profiles_LB->count(); i ++ ) {
      if( Profiles_LB->isSelected(i) ) {
        return Possible.at(i)->number();

      }
    }
    return -1;
}
