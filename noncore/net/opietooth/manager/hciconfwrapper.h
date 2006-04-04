/* $Id: hciconfwrapper.h,v 1.4 2006-04-04 12:16:09 korovkin Exp $ */
/* hcid.conf parser */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef HCICONFWRAPPER_H
#define HCICONFWRAPPER_H

#include <qstring.h>
#include <qstringlist.h>

namespace OpieTooth {

    class HciConfWrapper {

    public:
        HciConfWrapper( const QString &fileName );
        ~HciConfWrapper();
        void load();
        void save();

        void setPinHelper( const QString& app  );
        void setName( const QString& name );
        void setIscan( bool enable );
        void setPscan( bool enable );
        void setAuth( bool enable);
        void setEncrypt( bool enable);

    private:

        void setValue(const QString &entry, const  QString &value );

        QString m_fileName;
        QStringList m_file;
    };

}

#endif
