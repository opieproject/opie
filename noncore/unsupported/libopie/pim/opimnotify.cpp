#include <qshared.h>

#include "opimnotify.h"

struct OPimNotify::Data : public QShared {
    Data() : QShared(),dur(-1),parent(0) {

    }
    QDateTime start;
    int dur;
    QString application;
    int parent;
};

OPimNotify::OPimNotify( const QDateTime& start, int duration, int parent ) {
    data = new Data;
    data->start = start;
    data->dur = duration;
    data->parent = parent;
}
OPimNotify::OPimNotify( const OPimNotify& noti)
    : data( noti.data )
{
    data->ref();
}
OPimNotify::~OPimNotify() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}

OPimNotify &OPimNotify::operator=( const OPimNotify& noti) {
    noti.data->ref();
    deref();
    data = noti.data;

    return *this;
}
bool OPimNotify::operator==( const OPimNotify& noti ) {
    if ( data == noti.data ) return true;
    if ( data->dur != noti.data->dur ) return false;
    if ( data->parent != noti.data->parent ) return false;
    if ( data->application != noti.data->application ) return false;
    if ( data->start != noti.data->start ) return false;

    return true;
}
QDateTime OPimNotify::dateTime()const {
    return data->start;
}
QString OPimNotify::service()const {
    return data->application;
}
int OPimNotify::parent()const {
    return data->parent;
}
int OPimNotify::duration()const {
    return data->dur;
}
QDateTime OPimNotify::endTime()const {
    return QDateTime( data->start.date(), data->start.time().addSecs( data->dur) );
}
void OPimNotify::setDateTime( const QDateTime& time ) {
    copyIntern();
    data->start = time;
}
void OPimNotify::setDuration( int dur ) {
    copyIntern();
    data->dur = dur;
}
void OPimNotify::setParent( int uid ) {
    copyIntern();
    data->parent = uid;
}
void OPimNotify::setService( const QString& str ) {
    copyIntern();
    data->application = str;
}
void OPimNotify::copyIntern() {
    if ( data->count != 1 ) {
        data->deref();
        Data* dat = new Data;
        dat->start = data->start;
        dat->dur   = data->dur;
        dat->application = data->application;
        dat->parent = data->parent;
        data = dat;
    }
}
void OPimNotify::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}

/***********************************************************/
struct OPimAlarm::Data : public QShared {
    Data() : QShared() {
        sound = 1;
    }
    int sound;
    QString file;
};
OPimAlarm::OPimAlarm( int sound, const QDateTime& start, int duration, int parent )
    : OPimNotify( start, duration, parent )
{
    data = new Data;
    data->sound = sound;
}
OPimAlarm::OPimAlarm( const OPimAlarm& al)
    : OPimNotify(al), data( al.data )
{
    data->ref();
}
OPimAlarm::~OPimAlarm() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
OPimAlarm &OPimAlarm::operator=( const OPimAlarm& al)
{
    OPimNotify::operator=( al );
    deref();
    al.data->ref();

    data = al.data;


    return *this;
}
bool OPimAlarm::operator==( const OPimAlarm& al) {
    if ( data->sound != al.data->sound ) return false;
    else if ( data->sound == Custom && data->file != al.data->file )
        return false;

    return OPimNotify::operator==( al );
}
QString OPimAlarm::type()const {
    return QString::fromLatin1("OPimAlarm");
}
int OPimAlarm::sound()const {
    return data->sound;
}
QString OPimAlarm::file()const {
    return data->file;
}
void OPimAlarm::setSound( int snd) {
    copyIntern();
    data->sound = snd;
}
void OPimAlarm::setFile( const QString& sound ) {
    copyIntern();
    data->file = sound;
}
void OPimAlarm::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
void OPimAlarm::copyIntern() {
    if ( data->count != 1 ) {
        data->deref();
        Data *newDat = new Data;
        newDat->sound = data->sound;
        newDat->file  = data->file;
        data = newDat;
    }
}
/************************/
struct OPimReminder::Data : public QShared {
    Data() : QShared(), record( 0) {
    }
    int record;

};
OPimReminder::OPimReminder( int uid, const QDateTime& start,  int dur, int parent )
    : OPimNotify( start, dur, parent )
{
    data = new Data;
    data->record = uid;
}
OPimReminder::OPimReminder( const OPimReminder& rem )
    :  OPimNotify( rem ), data( rem.data )
{
    data->ref();
}
OPimReminder& OPimReminder::operator=( const OPimReminder& rem) {
    OPimNotify::operator=(rem );

    deref();
    rem.data->ref();
    data = rem.data;

    return *this;
}
bool OPimReminder::operator==( const OPimReminder& rem) {
    if ( data->record != rem.data->record ) return false;

    return OPimNotify::operator==( rem );
}
QString OPimReminder::type()const {
    return QString::fromLatin1("OPimReminder");
}
int OPimReminder::recordUid()const {
    return data->record;
}
void OPimReminder::setRecordUid( int uid ) {
    copyIntern();
    data->record = uid;
}
void OPimReminder::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}
void OPimReminder::copyIntern() {
    if ( data->count != 1 ) {
        Data* da = new Data;
        da->record = data->record;
        data = da;
    }
}
