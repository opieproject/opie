/**********************************************************************
** MNetwork* classes
**
** Encapsulates network information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef NETWORKINFO_H
#define NETWORKINFO_H

#include <qstring.h>
#include <qdict.h>

//---------------------------------------------------------------------------
// class MNetworkInterface
//

class MNetworkInterface
{
public:
        
    MNetworkInterface( const char* name = "eth0" );
    virtual ~MNetworkInterface();

    bool isLoopback() { return isLoopbackInterface; };
    const QString& getName() { return name; };

    virtual void updateStatistics();   

protected:
        
    int fd;
    const QString name;
    bool isLoopbackInterface;
    bool isIrda;
    bool isTunnel;
};

//---------------------------------------------------------------------------
// class MWirelessNetworkInterface
//

class MWirelessNetworkInterface : public MNetworkInterface
{
public:
    MWirelessNetworkInterface( const char* name = "wlan0" );
    virtual ~MWirelessNetworkInterface();
    
    int noisePercent();
    int qualityPercent();
    int signalPercent();

    QString APAddr;
    QString essid;
    QString mode;
    QString nick;
    QString rate;
    double freq;
    int channel;

    virtual void updateStatistics();
   
private: 
    int quality;
    int signal;
    int noise;

    bool hasWirelessExtensions;   
};

//---------------------------------------------------------------------------
// class MNetwork
//

class MNetwork
{
public:
    MNetwork();
    virtual ~MNetwork();

    typedef QDict<MNetworkInterface> InterfaceMap;
    typedef QDictIterator<MNetworkInterface> InterfaceMapIterator;

    bool hasInterfaces() const { return interfaces.isEmpty(); };
    int numInterfaces() const { return interfaces.count(); };

    MNetworkInterface* getFirstInterface();
    
protected:
    QString procfile;
    InterfaceMap interfaces;

    virtual MNetworkInterface* createInterface( const char* name ) const;

private:
    void enumerateInterfaces();
};

//---------------------------------------------------------------------------
// class MWirelessNetwork
//

class MWirelessNetwork : public MNetwork
{
public:
    MWirelessNetwork();
    virtual ~MWirelessNetwork();

protected:
    virtual MNetworkInterface* createInterface( const char* name )
        const;
};

#endif
