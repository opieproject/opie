/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H
#include "packagemanagerbase.h"
#include <qintdict.h>

class PackageItem;
class PackageDetails;
class PackageManagerSettings;

class PackageManager : public PackageManagerBase
{ 
    Q_OBJECT

public:
    PackageManager( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PackageManager();

    
public slots:
    void doDetails();
    void doSettings();
    void doFind();
    void doUpgrade();
    void setDocument(const QString& fileref);

protected slots:
    void doIt();
    
private slots:
    void installCurrent();
    void removeCurrent();
    void doNextDetails();

    void newServer();
    void editServer(int);
    void removeServer();
    void nameChanged(const QString&);
    void urlChanged(const QString&);
    void updatePackageList();

private:
    void maybeInstall( const QString &ipk );
    void startRun();
    void endRun();
    void startMultiRun(int jobs);
    int runIpkg(const QString& args);
    QString fullDetails(const QString& pk);

    bool readIpkgConfig(const QString& conffile);

    void doCurrentDetails(bool);
    PackageItem* current() const;

    QStringList findPackages( const QRegExp& re );
    void selectPackages( const QStringList& l );

    bool commitWithIpkg();

    QString ipkgStatusOutput();
    QString ipkgInfoOutput();
    void setCachedIpkgOutputDirty();

    PackageManagerSettings* settings;
    QIntDict<QString> serverurl;
    int editedserver;
    int ipkg_old;
    void writeSettings();
    void readSettings();

    PackageDetails* details;
    QCString cachedIpkgStatusOutput;
    QCString cachedIpkgInfoOutput;
};

#endif // PACKAGEMANAGER_H
