#ifndef BTICONLOADER_H
#define BTICONLOADER_H

#include <qpixmap.h>
#include <qmap.h>

namespace OpieTooth {

    class BTIconLoader {

    public:

        BTIconLoader();
        ~BTIconLoader();

        /**
         * Returns an icon depending on device class
         * @param deviceClass the device class id
         * @return the pixmap
         */
        QPixmap deviceIcon( const QString &deviceClass );

        /**
         * Returns an icon depending on service id
         * @param serviceClass the service id
         * @return the pixmap
         */
        QPixmap serviceIcon( const QString &serviceClass );

    private:
        // first ist id, second is icon name
        QMap<QString,QString> deviceIcons;
        QMap<QString,QString> serviceIcons;
    };
}

#endif
