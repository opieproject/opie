#include "interfaceppp.h"
#include "modem.h"
#include "pppdata.h"

InterfacePPP::InterfacePPP(QObject *parent, const char *name, bool status)
    : Interface(parent, name, status),
      _modem(0),
      _pppdata(0)
{
    qDebug("InterfacePPP::InterfacePPP(");
}

PPPData* InterfacePPP::data()
{
    if (!_pppdata){
        _pppdata = new PPPData();
        _pppdata->setModemDevice( getInterfaceName() );
        _pppdata->setAccount( getHardwareName() );
    }
    return _pppdata;
}

Modem* InterfacePPP::modem()
{
    if (!_modem){
        _modem = new Modem( data() );
    }
    return _modem;
}

bool InterfacePPP::refresh()
{
    qDebug("InterfacePPP::refresh()");
    return false;
}

void InterfacePPP::start()
{
    qDebug("InterfacePPP::start");

}

void InterfacePPP::stop()
{
    qDebug("InterfacePPP::stop");

}

