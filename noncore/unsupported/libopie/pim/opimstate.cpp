#include <qshared.h>

#include "opimstate.h"

/*
 * for one int this does not make
 * much sense but never the less
 * we will do it for the future
 */
struct OPimState::Data : public QShared {
    Data() : QShared(),state(Undefined) {
    }
    int state;
};

OPimState::OPimState( int state ) {
    data = new Data;
    data->state = state;
}
OPimState::OPimState( const OPimState& st) :
    data( st.data ) {
    /* ref up */
    data->ref();
}
OPimState::~OPimState() {
    if ( data->deref() ) {
        delete data ;
        data = 0;
    }
}
bool OPimState::operator==( const OPimState& st) {
    if ( data->state == st.data->state ) return true;

    return false;
}
OPimState &OPimState::operator=( const OPimState& st) {
    st.data->ref();
    deref();
    data = st.data;

    return *this;
}
void OPimState::setState( int st) {
    copyInternally();
    data->state = st;
}
int OPimState::state()const {
    return data->state;
}
void OPimState::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
void OPimState::copyInternally() {
    /* we need to change it */
    if ( data->count != 1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->state = data->state;
        data = d2;
    }
}
