/* $Id: btconfhandler.h,v 1.1 2006-04-20 12:37:33 korovkin Exp $ */
/* Bluetooth services configuration file handler */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _BTCONFHANDLER_H_
#define _BTCONFHANDLER_H_
#include <qstring.h>
#include <qstringlist.h>

namespace OpieTooth {
    class BTConfHandler {
    public:
        BTConfHandler(const QString& conf);
        ~BTConfHandler();
        bool saveConfig();

        bool getHcidEnable() { return hcidEnable; }
        void setHcidEnable(bool arg) { hcidEnable = arg; }

        bool getHiddEnable() { return hiddEnable; }
        void setHiddEnable(bool arg) { hiddEnable = arg; }

        bool getSdpdEnable() { return sdpdEnable; }
        void setSdpdEnable(bool arg) { sdpdEnable = arg; }

        bool getRfcommEnable() { return rfcommEnable; }
        void setRfcommEnable(bool arg) { rfcommEnable = arg; }

        //DUND parameter
        bool getDundEnable() { return dundEnable; }
        void setDundEnable(bool arg) { dundEnable = arg; }
        bool getDunDisableSDP() { return dundDisableSDP; }
        void setDunDisableSDP(bool arg) { dundDisableSDP = arg; }
        bool getDunPersist() { return dundPersist; }
        void setDunPersist(bool arg) { dundPersist = arg; }
        QString getDunPersistPeriod() { return dundPersistPeriod; }
        void setDunPersistPeriod(QString& arg) { dundPersistPeriod = arg; }
        bool getDunCache() { return dundCache; }
        void setDunCache(bool arg) { dundCache = arg; }
        QString getDunCachePeriod() { return dundCachePeriod; }
        void setDunCachePeriod(QString& arg) { dundCachePeriod = arg; }
        bool getDunMsdun() { return dundMsdun; }
        void setDunMsdun(bool arg) { dundMsdun = arg; }
        QString getDunMsdunTimeout() { return dundMsdunTimeout; }
        void setDunMsdunTimeout(QString& arg) { dundMsdunTimeout = arg; }
        QString getDunChannel() { return dundChannel; }
        void setDunChannel(QString& arg) { dundChannel = arg; }
        bool getDunPPPd() { return dundPPPd; }
        void setDunPPPd(bool arg) { dundPPPd = arg; }
        QString getDunPPPdPath() { return dundPPPdPath; }
        void setDunPPPdPath(QString& arg) { dundPPPdPath = arg; }
        QString getDunPPPdOptions() { return dundPPPdOptions; }
        void setDunPPPdOptions(QString& arg) { dundPPPdOptions = arg; }

        //PAND parameters
        bool getPanEnable() { return pandEnable; }
        void setPanEnable(bool arg) { pandEnable = arg; }
        bool getPanEncrypt() { return pandEncrypt; }
        void setPanEncrypt(bool arg) { pandEncrypt = arg; }
        bool getPanSecure() { return pandSecure; }
        void setPanSecure(bool arg) { pandSecure = arg; }
        bool getPanMaster() { return pandMaster; }
        void setPanMaster(bool arg) { pandMaster = arg; }
        bool getPanDisableSDP() { return pandDisableSDP; }
        void setPanDisableSDP(bool arg) { pandDisableSDP = arg; }
        QString getPanRole() { return pandRole; }
        void setPanRole(QString& arg) { pandRole = arg; }
        bool getPanPersist() { return pandPersist; }
        void setPanPersist(bool arg) { pandPersist = arg; }
        QString getPanPersistPeriod() { return pandPersistPeriod; }
        void setPanPersistPeriod(QString& arg) { pandPersistPeriod = arg; }
        bool getPanCache() { return pandCache; }
        void setPanCache(bool arg) { pandCache = arg; }
        QString getPanCachePeriod() { return pandCachePeriod; }
        void setPanCachePeriod(QString& arg) { pandCachePeriod = arg; }

        QString& getConfName() { return confName; }
    protected:
        bool getBoolArg(QString& str, QCString& keyword);
        QString getStringArg(QString& str, QCString& keyword);
        bool readConfig(const QString& conf);
        void parsePanParameters(const QString& conf);
        void parseDunParameters(const QString& conf);
    protected:
        bool hcidEnable;
        bool hiddEnable;
        bool sdpdEnable;
        bool rfcommEnable;
        //DUND parameters
        bool dundEnable;
        bool dundDisableSDP;
        bool dundPersist;
        QString dundPersistPeriod;
        bool dundCache;
        QString dundCachePeriod;
        bool dundMsdun;
        QString dundMsdunTimeout;
        QString dundChannel;
        bool dundPPPd;
        QString dundPPPdPath;
        QString dundPPPdOptions;

        //PAND parameters
        bool pandEnable;
        bool pandEncrypt;
        bool pandSecure;
        bool pandMaster;
        bool pandDisableSDP;
        bool pandPersist;
        QString pandPersistPeriod;
        bool pandCache;
        QString pandCachePeriod;
        QString pandRole;
        QStringList list; //list of strings we read from configuration file
        QString confName; //Configuration filename
    };
};
#endif
//eof
