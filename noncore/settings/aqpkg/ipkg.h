/***************************************************************************
                          ipkg.h  -  description
                             -------------------
    begin                : Sat Aug 31 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IPKG_H
#define IPKG_H


/**
  *@author Andy Qua
  */

#include <qobject.h>
#include <qstring.h>
#include <qlist.h>

#define FORCE_DEPENDS                           0x0001
#define FORCE_REMOVE                            0x0002
#define FORCE_REINSTALL                         0x0004
#define FORCE_OVERWRITE                         0x0008
#define MAKE_LINKS                              0x0010
  
class Ipkg : public QObject
{
    Q_OBJECT
public:
    Ipkg();
    ~Ipkg();
    bool runIpkg( );

    void setOption( const char *opt )               { option = opt; }
    void setPackage( const char *pkg )              { package = pkg; }
    void setDestination( const char *dest )         { destination = dest; }
    void setDestinationDir( const char *dir )       { destDir = dir; }
    void setFlags( int fl )                         { flags = fl; }
    void setRuntimeDirectory( const char *dir )     { runtimeDir = dir; }

signals:
    void outputText( const QString &text );
    
private:
    bool createLinks;
    QString option;
    QString package;
    QString destination;
    QString destDir;
    int flags;
    QString runtimeDir;

    QList<QString> *dependantPackages;
    
    int executeIpkgCommand( QString &cmd, const QString option );
    void removeStatusEntry();
    void linkPackage( const QString &packFileName, const QString &dest, const QString &destDir );
    QStringList* getList( const QString &packageFilename, const QString &destDir );
    void processFileList( const QStringList *fileList, const QString &destDir );
    void processLinkDir( const QString &file, const QString &baseDir, const QString &destDir );
};

#endif
