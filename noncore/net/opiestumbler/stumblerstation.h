#ifndef STUMBLERSTATION_H
#define STUMBLERSTATION_H

#include <opie2/ostation.h>

#include <qdatetime.h>

class StumblerStation
{
public:
    StumblerStation(Opie::Net::OStation *station, const QDateTime &tm);
    ~StumblerStation();
    bool operator<(const StumblerStation &rhs);
    Opie::Net::OStation *st;
    QDateTime lastTimeSeen;
};

#endif
