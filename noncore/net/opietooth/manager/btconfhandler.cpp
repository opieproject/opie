/* $Id: btconfhandler.cpp,v 1.1 2006-04-20 12:37:33 korovkin Exp $ */
/* Bluetooth services configuration file handler */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "btconfhandler.h"
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <opie2/odebug.h>

//Keywords
static QCString k_hcidEnable("HCID_ENABLE=");
static QCString k_hiddEnable("HIDD_ENABLE=");
static QCString k_sdpdEnable("SDPD_ENABLE=");
static QCString k_rfcommEnable("RFCOMM_ENABLE=");
static QCString k_dundEnable("DUND_ENABLE=");
static QCString k_pandEnable("PAND_ENABLE=");
static QCString k_pandOpts("PAND_OPTIONS=");
static QCString k_dundOpts("DUND_OPTIONS=");

const struct option pandCLOpts[] = //Pand command line options
{
    { "listen", 0, NULL, 's' },
    { "encrypt", 0, NULL, 'E' },
    { "secure", 0, NULL, 'S' },
    { "master", 0, NULL, 'M' },
    { "nosdp", 0, NULL, 'D' },
    { "role", 1, NULL, 'r' },
    { "persist", 0, NULL, 'p' },
    { "cache", 0, NULL, 'C' },
    { NULL, 0, NULL, 0 } //Marker of the end
};

const struct option dundCLOpts[] = //Pand command line options
{
    { "listen", 0, NULL, 's' },
    { "nosdp", 0, NULL, 'D' },
    { "persist", 0, NULL, 'p' },
    { "cache", 0, NULL, 'C' },
    { "msdun", 0, NULL, 'X' },
    { "channel", 1, NULL, 'P' },
    { "pppd", 1, NULL, 'd' },
    { NULL, 0, NULL, 0 } //Marker of the end
};

using namespace OpieTooth;
BTConfHandler::BTConfHandler(const QString& conf): confName(conf)
{
    hcidEnable = false;
    hiddEnable = false;
    sdpdEnable = false;
    rfcommEnable = false;
    
    //dund parameters
    dundEnable = false;
    dundDisableSDP = false;
    dundPersist = false;
    dundPersistPeriod = "";
    dundCache = false;
    dundCachePeriod = "";
    dundMsdun = false;
    dundMsdunTimeout = "";
    dundChannel = "1";
    dundPPPd = false;
    dundPPPdPath = "";
    dundPPPdOptions = "";
    
    //pandparameters
    pandEnable = false;
    pandEncrypt = false;
    pandSecure = false;
    pandMaster = false;
    pandDisableSDP = false;
    pandRole = "";
    pandPersist = false;
    pandPersistPeriod = "";
    pandCache = false;
    pandCachePeriod = "";

    readConfig(confName);
}

BTConfHandler::~BTConfHandler()
{
}

/**
 * getBoolArg
 * Gets boolean argument from a string str with a keyword key
 */
bool BTConfHandler::getBoolArg(QString& str, QCString& keyword)
{
    QString arg = str.mid(keyword.length(), str.length() - keyword.length());
    return (bool)(arg == "true");
}

/**
 * getStringArg
 * Gets string argument from a string str with a keyword key
 */
QString BTConfHandler::getStringArg(QString& str, QCString& keyword)
{
    QString arg = str.mid(keyword.length(), str.length() - keyword.length());
    if (arg.left(1) == "\"")
        arg.remove(0, 1);
    if (arg.right(1) == "\"")
        arg.remove(arg.length() - 1, 1);
    return arg;
}

/**
 * parsePanParameters
 * Function gets PAND parameters from the command line conf
 */
void BTConfHandler::parsePanParameters(const QString& conf)
{
    int i; //just an index variable
    int argc;
    QStringList panList = QStringList::split(" ", conf);
    argc = panList.count();
    char* argv[argc + 1];
    int shOpt;
    i = 1;
    optind = 0;
    argv[0] = strdup("pand");
    for (QStringList::Iterator arg = panList.begin(); 
        arg != panList.end() && i < argc + 1; arg++, i++)  {
        argv[i] = strdup((*arg));
    }
    do {
        shOpt = getopt_long(argc + 1, argv, "sESMDr:p:C:", pandCLOpts, NULL);
        switch(shOpt) {
            case 'E':
                pandEncrypt = true;
                break;
            case 'S':
                pandSecure = true;
                break;
            case 'M':
                pandMaster = true;
                break;
            case 'D':
                pandDisableSDP = true;
                break;
            case 'r':
                if (optarg)
                    pandRole = optarg;
                else
                    pandRole = "NG";
                break;
            case 'p':
                pandPersist = true;
                if (optarg)
                    pandPersistPeriod = optarg;
                break;
            case 'C':
                pandCache = true;
                if (optarg)
                    pandCachePeriod = optarg;
                break;
            case 's':
            case '?':
            default:
                break;
        }
    } while (shOpt != -1);
    for (i = 0; i < argc + 1; i++) {
        free(argv[i]);
    }
}

/**
 * parseDunParameters
 * Function gets PAND parameters from the command line conf
 */
void BTConfHandler::parseDunParameters(const QString& conf)
{
    int i; //just an index variable
    int argc;
    QStringList dunList = QStringList::split(" ", conf);
    argc = dunList.count();
    char* argv[argc + 1];
    int shOpt;
    i = 1;
    optind = 0;
    argv[0] = strdup("dund");
    for (QStringList::Iterator arg = dunList.begin(); 
        arg != dunList.end() && i < argc + 1; arg++, i++)  {
        argv[i] = strdup((*arg));
    }
    do {
        shOpt = getopt_long(argc + 1, argv, "sDp:C:X:P:d:", dundCLOpts, NULL);
        switch(shOpt) {
            case 'D':
                dundDisableSDP = true;
                break;
            case 'p':
                dundPersist = true;
                if (optarg)
                    dundPersistPeriod = optarg;
                break;
            case 'C':
                dundCache = true;
                if (optarg)
                    dundCachePeriod = optarg;
                break;
            case 'X':
                dundMsdun = true;
                if (optarg)
                    dundMsdunTimeout = optarg;
                break;
            case 'P':
                if (optarg)
                    dundChannel = optarg;
                else
                    dundChannel = "1";
                break;
            case 'd':
                dundPPPd = true;
                if (optarg)
                    dundPPPdPath = optarg;
                break;
            case 's':
            case '?':
            default:
                break;
        }
    } while (shOpt != -1);
    if (optind < argc + 1) {
        for (i = optind; i < argc + 1; i++) {
            if (!dundPPPdOptions.isEmpty())
                dundPPPdOptions += " ";
            dundPPPdOptions += argv[i];
        }
    }
    for (i = 0; i < argc + 1; i++) {
        free(argv[i]);
    }
}

/**
 * readConfig
 * Reads configuration file with conf as a file name
 * return true on success and false on failure
 */
bool BTConfHandler::readConfig(const QString& conf)
{
    QFile btConf(conf); //File we read
    QString tmp; //temporary string
    if (btConf.open(IO_ReadOnly))  {
        QTextStream inStream(&btConf);
        list = QStringList::split("\n", inStream.read(), true);
        for (QStringList::Iterator line = list.begin(); 
            line != list.end(); line++)  {
            QString tmpLine = (*line).simplifyWhiteSpace();
            if (tmpLine.startsWith("#"))
                continue;
            else if (tmpLine.startsWith(k_hcidEnable))
                hcidEnable = getBoolArg(tmpLine, k_hcidEnable);
            else if (tmpLine.startsWith(k_hiddEnable))
                hiddEnable = getBoolArg(tmpLine, k_hiddEnable);
            else if (tmpLine.startsWith(k_sdpdEnable))
                sdpdEnable = getBoolArg(tmpLine, k_sdpdEnable);
            else if (tmpLine.startsWith(k_rfcommEnable))
                rfcommEnable = getBoolArg(tmpLine, k_rfcommEnable);
            else if (tmpLine.startsWith(k_dundEnable))
                dundEnable = getBoolArg(tmpLine, k_dundEnable);
            else if (tmpLine.startsWith(k_pandEnable))
                pandEnable = getBoolArg(tmpLine, k_pandEnable);
            else if (tmpLine.startsWith(k_pandOpts)) {
                tmp = getStringArg(tmpLine, k_pandOpts);
                parsePanParameters(tmp);
            }
            else if (tmpLine.startsWith(k_dundOpts)) {
                tmp = getStringArg(tmpLine, k_dundOpts);
                parseDunParameters(tmp);
            }
        }
        return true;
    }
    return false;
}

/**
 * writeConfig
 * Writes configuration file with conf as a file name
 * return true on success and false on failure
 */
bool BTConfHandler::saveConfig()
{
    QFile btConf(confName); //File we read
    if (!btConf.open(IO_WriteOnly))
        return false;
    QTextStream stream(&btConf);
    for (QStringList::Iterator line = list.begin(); 
        line != list.end(); line++)  {
        QString tmpLine = (*line).simplifyWhiteSpace();
        if (tmpLine.startsWith(k_hcidEnable))
            (*line) = k_hcidEnable + ((hcidEnable)? "true": "false");
        else if (tmpLine.startsWith(k_hiddEnable))
            (*line) = k_hiddEnable + ((hiddEnable)? "true": "false");
        else if (tmpLine.startsWith(k_sdpdEnable))
            (*line) = k_sdpdEnable + ((sdpdEnable)? "true": "false");
        else if (tmpLine.startsWith(k_rfcommEnable))
            (*line) = k_rfcommEnable + ((rfcommEnable)? "true": "false");
        else if (tmpLine.startsWith(k_dundEnable))
            (*line) = k_dundEnable + ((dundEnable)? "true": "false");
        else if (tmpLine.startsWith(k_pandEnable))
            (*line) = k_pandEnable + ((pandEnable)? "true": "false");
        else if (tmpLine.startsWith(k_pandOpts)) {
            (*line) = k_pandOpts + "\"--listen";
            (*line) += " --role " + pandRole;
            if (pandEncrypt)
                (*line) += " --encrypt";
            if (pandSecure)
                (*line) += " --secure";
            if (pandMaster)
                (*line) += " --master";
            if (pandDisableSDP)
                (*line) += " --nosdp";
            if (pandPersist) {
                if (pandPersistPeriod.isEmpty())
                    (*line) += " --persist";
                else {
                    (*line) += " -p ";
                    (*line) += pandPersistPeriod;
                }
            }
            if (pandCache) {
                if (pandCachePeriod.isEmpty())
                    (*line) += " --cache";
                else {
                    (*line) += " -C ";
                    (*line) += pandCachePeriod;
                }
            }
            (*line) += "\"";
        }
        else if (tmpLine.startsWith(k_dundOpts)) {
            (*line) = k_dundOpts + "\"--listen";
            (*line) += " --channel " + dundChannel;
            if (dundDisableSDP)
                (*line) += " --nosdp";
            if (dundPersist) {
                if (dundPersistPeriod.isEmpty())
                    (*line) += " --persist";
                else {
                    (*line) += " -p ";
                    (*line) += dundPersistPeriod;
                }
            }
            if (dundCache) {
                if (dundCachePeriod.isEmpty())
                    (*line) += " --cache";
                else {
                    (*line) += " -C ";
                    (*line) += dundCachePeriod;
                }
            }
            if (dundPPPd)
                (*line) += " --pppd " + dundPPPdPath;
            if (dundMsdun) {
                if (dundMsdunTimeout.isEmpty())
                    (*line) += " --msdun";
                else {
                    (*line) += " -X ";
                    (*line) += dundMsdunTimeout;
                }
            }
            if (!dundPPPdOptions.isEmpty()) {
                (*line) += " ";
                (*line) += dundPPPdOptions;
            }
            (*line) += "\"";
        }
        stream << (*line) << endl;
    }
    return true;
}

//eof
