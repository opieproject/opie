/***************************************************************************
                          utils.h  -  description
                             -------------------
    begin                : Sat Sep 7 2002
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

#ifndef UTILS_H
#define UTILS_H

#include <qstring.h>

/**
  *@author Andy Qua
  */

class Utils {
public: 
	Utils();
	~Utils();

    static QString getPathfromIpkFilename( const QString &file );
    static QString getFilenameFromIpkFilename( const QString &file );
    static QString getPackageNameFromIpkFilename( const QString &file );
    static QString getPackageVersionFromIpkFilename( const QString &file );
    static bool getStorageSpace( const char *path, long *blockSize, long *totalBlocks, long *availBlocks );
};

#endif
