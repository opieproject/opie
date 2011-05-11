/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "rfkill.h"

/* OPIE */
#include <opie2/odebug.h>

/* QT */
#include <qmap.h>

#include <sysfs/libsysfs.h>

using namespace Opie::Core;

ORfKill::ORfKill()
    : QObject()
{
}

ORfKill::~ORfKill()
{
}

QString ORfKill::findNode()
{
    if( !m_device.isEmpty() )
        return m_device;

    QString device = "";
    struct sysfs_class *cls = sysfs_open_class( "rfkill" );
    if( cls ) {
        QStringList props;
        props += "type";
        props += "persistent";

        struct dlist *cdevs = sysfs_get_class_devices( cls );
        if (cdevs != NULL) {
            struct sysfs_class_device *cdev = NULL;
            dlist_for_each_data(cdevs, cdev, struct sysfs_class_device) {

                struct dlist *attrlist = sysfs_get_classdev_attributes( cdev );
                if (attrlist != NULL) {
                    QMap<QString,QString> propmap;
                    struct sysfs_attribute *attr = NULL;
                    dlist_for_each_data(attrlist, attr, struct sysfs_attribute) {
                        if( props.findIndex( attr->name ) > -1 ) {
                            if( sysfs_read_attribute( attr ) == 0 )
                                propmap.insert( attr->name, QString( attr->value ).stripWhiteSpace() );
                        }
                    }
                    odebug << QString(cdev->name) << " " << propmap["type"] << " " << propmap["persistent"] << oendl;
                    if( propmap["type"] == "bluetooth" ) {
                        if( !device.isEmpty() )
                            device = cdev->name;
                        if( propmap["persistent"].toInt() ) {
                            m_device = cdev->name;
                            return m_device;
                        }
                    }
                }
            }
        }
        sysfs_close_class( cls );
    }
    return device;
}

bool ORfKill::kill()
{
    QString device = findNode();
    if( !device.isEmpty() ) {
        struct sysfs_class_device *dev = sysfs_open_class_device("rfkill", device);
        if( dev ) {
            struct sysfs_attribute *attr = sysfs_get_classdev_attr(dev, "soft");
            if( sysfs_write_attribute(attr, "1", 1) == 0 ) {
                return true;
            }
        }
    }
    return false;
}

bool ORfKill::restore()
{
    QString device = findNode();
    if( !device.isEmpty() ) {
        struct sysfs_class_device *dev = sysfs_open_class_device("rfkill", device);
        if( dev ) {
            struct sysfs_attribute *attr = sysfs_get_classdev_attr(dev, "soft");
            if( sysfs_write_attribute(attr, "0", 1) == 0 ) {
                return true;
            }
        }
    }
    return false;
}

bool ORfKill::isAvailable()
{
    return( !findNode().isEmpty() );
}
