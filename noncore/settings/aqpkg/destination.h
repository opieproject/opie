/***************************************************************************
                          destination.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
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
#ifndef DESTINATION_H
#define DESTINATION_H

#include <qstring.h>

class Destination
{
public:
    Destination() {}
    Destination( const char *name, const char *path );
    ~Destination();

    void setDestinationName( const QString &name ) { destName = name; }
    void setDestinationPath( const QString &path )   { destPath = path; }
    void linkToRoot( bool val )             { linkToRootDir = val; }

    bool linkToRoot()             { return linkToRootDir; }
    QString &getDestinationName() { return destName; }
    QString &getDestinationPath()  { return destPath; }

protected:

private:
  QString destName;
  QString destPath;
  bool linkToRootDir;
};

#endif
