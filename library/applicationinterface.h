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


/**
 * \brief Application interface currently used by the quicklaunch framework
 *
 *  This is the interface to be exposed by applications available as DSO
 *  Normally one would use the OApplicationFactory which does the magic of
 *  exposing the interface.
 *
 *
 *  Resulting dynamic shared objects (dso) need to go into the
 *  OPIEDIR/plugins/application.
 *
 *
 *  You can use this interface to load applications into your application.
 *  @todo  Implement Services + Trader
 *  @since Opie 1.0.2
 */
struct ApplicationInterface : public QUnknownInterface
{
public:

    /**
     * \brief create the mainwindow for the giving application name
     * Create a main window for the giving application name
     *
     * @param appName The application widget to be created
     * @param parent The parent of the newly created widget
     * @param name The name of the QObject
     * @param f Possible Window Flags
     *
     * @return the widget or 0l
     */
    virtual QWidget *createMainWindow( const QString &appName, QWidget *parent=0,
                                       const char *name=0, Qt::WFlags f=0 ) = 0;

   /**
    * The list of application windows supported
    */
    virtual QStringList applications() const = 0;
};

/*
 * Use an extended interface for QObejct, Opie::Part in the future
 */

#endif
