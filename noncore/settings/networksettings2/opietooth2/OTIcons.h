#ifndef OTICONLOADER_H
#define OTICONLOADER_H

#include <OTUUID.h>
#include <qpixmap.h>
#include <qmap.h>

namespace Opietooth2 {

class OTIcons {

public:

    OTIcons();
    ~OTIcons();

    /**
     * Returns an icon depending on device class
     * @param deviceClass the device class name
     * @return the pixmap
     */
    QPixmap deviceIcon( const QString & );

    /**
     * Returns an icon depending on service id
     * @param serviceClass the service id
     * @return the pixmap
     * @return true if found
     */
    QPixmap serviceIcon( int, bool & );

    // returns all UUID that represent channels with modem function
    const UUIDVector & modems()
      { return Modems; }

    // returns all UUID that represent channels with network
    const UUIDVector & network()
      { return Networks; }

    // set Sub to find icons in .../Icons dir
    QPixmap loadPixmap( const QString &, bool Sub = 0 );

private:

    // first ist id, second is icon name
    QMap<QString,QString> deviceIcons;
    QMap<int,QString> serviceIcons;
    UUIDVector Modems;
    UUIDVector Networks;

};
};

#endif
