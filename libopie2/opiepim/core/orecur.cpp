#include <qshared.h>

#include <qtopia/timeconversion.h>

#include "orecur.h"

struct ORecur::Data : public QShared {
    Data() : QShared() {
        type = ORecur::NoRepeat;
        freq = -1;
        days = 0;
        pos = 0;
        create = -1;
        hasEnd = FALSE;
        end = 0;
    }
    char days; // Q_UINT8 for 8 seven days;)
    ORecur::RepeatType type;
    int freq;
    int pos;
    bool hasEnd : 1;
    time_t end;
    time_t create;
};


ORecur::ORecur() {
    data = new Data;
}
ORecur::ORecur( const ORecur& rec)
    : data( rec.data )
{
    data->ref();
}
ORecur::~ORecur() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
void ORecur::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
bool ORecur::operator==( const ORecur& )const {
    return false;
}
ORecur &ORecur::operator=( const ORecur& re) {
    re.data->ref();
    deref();
    data = re.data;

    return *this;
}
ORecur::RepeatType ORecur::type()const{
    return data->type;
}
int ORecur::frequency()const {
    return data->freq;
}
int ORecur::position()const {
    return data->pos;
}
char ORecur::days() const{
    return data->days;
}
bool ORecur::hasEndDate()const {
    return data->hasEnd;
}
QDate ORecur::endDate()const {
    return TimeConversion::fromUTC( data->end ).date();
}
time_t ORecur::endDateUTC()const {
    return data->end;
}
time_t ORecur::createTime()const {
    return data->create;
}
void ORecur::setType( const RepeatType& z) {
    checkOrModify();
    data->type = z;
}
void ORecur::setFrequency( int freq ) {
    checkOrModify();
    data->freq = freq;
}
void ORecur::setPosition( int pos ) {
    checkOrModify();
    data->pos = pos;
}
void ORecur::setDays( char c ) {
    checkOrModify();
    data->days = c;
}
void ORecur::setEndDate( const QDate& dt) {
    checkOrModify();
    data->end = TimeConversion::toUTC( dt );
}
void ORecur::setEndDateUTC( time_t t) {
    checkOrModify();
    data->end = t;
}
void ORecur::setCreateTime( time_t t) {
    checkOrModify();
    data->create = t;
}
void ORecur::setHasEndDate( bool b) {
    checkOrModify();
    data->hasEnd = b;
}
void ORecur::checkOrModify() {
    if ( data->count !=  1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->days = data->days;
        d2->type = data->type;
        d2->freq = data->freq;
        d2->pos  = data->pos;
        d2->hasEnd = data->hasEnd;
        d2->end  = data->end;
        d2->create = data->create;
        data = d2;
    }
}

