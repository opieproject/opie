/*
 * GPLv2 (C) 2002-2003 Trolltech
 *       (C) 2003 zecke@handhelds.org
 */


#ifndef APPLICATIONINTERFACE_H
#define APPLICATIONINTERFACE_H

#include <qstringlist.h>
#include <qtopia/qcom.h>

#ifndef QT_NO_COMPONENT
// {07E15B48-B947-4334-B866-D2AD58157D8C}
#ifndef IID_QtopiaApplication
#define IID_QtopiaApplication QUuid( 0x07e15b48, 0xb947, 0x4334, 0xb8, 0x66, 0xd2, 0xad, 0x58, 0x15, 0x7d, 0x8c)
#endif
#endif

struct ApplicationInterface : public QUnknownInterface
{
public:
    virtual QWidget *createMainWindow( const QString &appName, QWidget *parent=0,
                                       const char *name=0, Qt::WFlags f=0 ) = 0;
    virtual QStringList applications() const = 0;
};

/*
 * Use an extended interface for QObejct, Opie::Part in the future
 */

#endif
