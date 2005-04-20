#include "stumblerstation.h"
#include <opie2/odebug.h>

StumblerStation::StumblerStation(Opie::Net::OStation *station, const QDateTime &tm)
    : st(station), lastTimeSeen(tm)
{
    /*odebug << "RHS: " << station->macAddress.toString() << oendl;
    odebug << "THIS: " << st->macAddress.toString() << oendl; */
}

StumblerStation::~StumblerStation()
{
    if (st)
        delete st;
}

bool StumblerStation::operator<(const StumblerStation &rhs)
{
    return rhs.lastTimeSeen < lastTimeSeen;
}

