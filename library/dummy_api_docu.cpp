/*
 * A place to explain various headers
 */

/*
 * applicationinterface.h
 */

/**
 * \class ApplicationInterface
 * \brief Application interface currently used by the quicklaunch framework
 *
 *  This is the interface to be exposed by applications available as DSO
 *  Normally one would use the OApplicationFactory which does the magic of
 *  exposing the interface.
 *
 *  Resulting dynamic shared objects (dso) need to go into the
 *  OPIEDIR/plugins/application.
 *
 *
 *  You can use this interface to load applications into your application.
 *  @todo  Implement Services + Trader
 *  @since Opie 1.0.2
 */
/**
 * \fn QWidget* ApplicationInterface::createMainWindow(const QString& appName, QWidget* parent, const char* name, Qt::WFlags f)
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
/**
 * \fn QStringList ApplicationInterface::applications()const
 * The list of application windows supported
 */

/*
 * Font Factory
 */

/**
 * \class FontFactoryInterface
 * \brief Interface for additional Font Factories
 *
 * To add special types of fonts TrueTypes or your very own
 * format. If you implement this Interface you can add
 * custom font loading.
 *
 * The dynamic shared object goes to
 * OPIEDIR/plugins/fontfactories.
 *
 * As of version Opie 1.0.2 loading of the plugins is not
 * yet implemented
 *
 */

/**
 * \fn QFontFactory* FontFactoryInterface::fontFactory()
 *
 * Create a new QFontFactory and return it
 */


/*
 * ImageCodec
 */

/**
 * \class ImageCodecInterface
 * \brief add Image Codecs
 *
 * This interface will be queried to add new Image loading
 * and saving routines.
 *
 * The ImageCodec needs to be placed in OPIEDIR/plugins/imagecodecs.
 *
 * @see QImage
 * @see QImageIO
 **/

/**
 * \fn QStringList ImageCodecInterface::keys()const
 * \brief Query for the list of supported formats
 *
 *  Return a QStringList of the supported formats
 *
 */

/**
 * \fn bool ImageCodecInterface::installIOHandler(const QString& format )
 * \brief install codec to QIageIO for the specefic format
 *
 * Install a QImage codec for the requested format
 */

/*
 * Input Methods
 */

/**
 * \class InputMethodInterface
 * \brief Interface class for inputting keyboard events
 *
 * InputMethods are loaded by the Launcher/Server/Taskbar
 * and are located inside OPIEDIR/plugins/inputmethods
 *
 * Depending on the device these InputMethods are the only way
 * to input charachters
 *
 */


/**
 * \fn QWidget InputMethodInterface::inputMethod(QWidget*parent, Qt::WFlags f)
 * \brief create a new widget which should be used as input
 *
 * This method will be called if the inputmethod is to be shown.
 * Make sure that your widget is not too large. As of Opie1.1 InputMethods
 * can be floating as well.
 *
 * Delete the Widget yourself.
 *
 *
 * @param parent The parent of the to be created Input widget.
 * @param f The Qt::WFlags for the widget
 */

/**
 * \fn void InputMethodInterface::resetState()
 * \brief Reset the state of the inputmethod
 *
 *  If you're shown reset the state of the keyboard to the
 *  the default.
 */

/**
 * \fn QPixmap* InputMethodInterface::icon()
 * \brief The icon of your Input method
 *
 * Return a pointer to a QPixmap symboling your inputmethod
 * You need to delete the pixmap later yourself.
 */

/**
 * \fn void InputMethodInterface::onKeyPress(QObject* receiver, const char* slot)
 * \brief pass your key event through
 *
 * In your actual Input Implementation you'll need a SIGNAL with this
 * void key(ushort,ushort,ushort,bool,bool) signal. The host of your input method
 * requests you to connect your signal with the signal out of receiver and slot.
 *
 * ushort == unicode value
 * ushort == keycode
 * ushort == modifiers from Qt::ButtonState
 * bool   == true if the key is pressed and false if released
 * bool   == autorepeat on or off.
 *
 * See the QWSServer for more information about emitting keys
 *
 *
 * @param receiver the receiver to QObject::connect to
 * @param slot the slot to QObject::connect to
 *
 */

/*
 * MediaPlayer Plugins
 */

/**
 * \class MediaPlayerPluginInterface
 * \brief Plugins for the Opie Player I
 *
 * You can extend the Opie Player I by plugins placed in
 * OPIEDIR/plugins/codecs
 *
 *
 */

/**
 * \fn MediaPlayerDecoder MediaPlayerPluginInterface::decoder()
 *
 * Create a new MediaPlayerDecoder
 *
 */


/*
 * MenuApplet Interface
 */

/**
 * \class MenuAppletInterface
 * \brief Plugins for the Menu Applet/StartMenu
 *
 *  You can extend the startmenu by plugins implementing this
 * interface. You need to place the plugin in plugins/applets
 * from where they will be loaded.
 *
 *
 */

/**
 * \fn QString MenuAppletInterface::name()const
 * \brief Translated name of the Menu Applet
 *
 * Return a translated name using QObject::tr  of your plugin
 */

/**
 * \fn int MenuAppletInterface::position()const
 * \brief the wished position of this applet
 *
 *  The position where you want to be placed. 0 for the down most
 *
 */

/**
 * \fn QIconSet MenuAppletInterface::icon()const
 * \brief return a QIconSet.
 *
 * The returned icon set will be shown next
 * to text().
 * Make use of AppLnk::smallIconSize()
 */

/**
 * \fn QString MenuAppletInterface::text()const
 * \brief return a Text shown to the user in the menu
 */

/**
 * \fn QPopupMenu* MenuAppletInterface::popup( QWidget* parent)const
 * \brief Provide a SubMenu popup if you want
 *
 * You can provide a Submenu popup for your item as well. If you return
 * 0 no popup will be shown.
 *
 * You can use the QPopupMenu::aboutToShow() signal to be informed before
 * showing the popup
 *
 * @param parent The parent of the to be created popup.
 * @see QPopupMenu
 */


/**
 * \fn void MenuAppletInterface::activated()
 * \brief This method gets called once the user clicked on the item
 *
 * This is the way you get informed about user input. Your plugin
 * has just been clicked
 */

/*
 * StyleInterface
 */

/**
 * \class StyleInterface
 * \brief StyleInterface base class
 *
 * Opie styles should implement StyleExtendedInterface.
 * StyleInterface is only for compability reasons present and should
 * not be used for new styles.
 *
 * Styles need to be put into OPIEDIR/plugins/styles
 */


/**
 * \class StyleExtendedInterface
 * \brief The Plugin Interface for all Opie styles
 *
 * If you want to create a new QStyle for Opie use this class.
 *
 * key(ushort,ushort,ushort,bool,bool)
 */

/*
 * Taskbar Applets
 */


/**
 * \class TaskbarAppletInterface
 *
 * This is the base class of all Applets shown in the taskbar
 * An applets need to provide a position and a widget.
 *
 * Applets need to be put into OPIEDIR/plugins/applets
 *
 */
/**
 * \fn QWidget* TaskbarAppletInterface::applet( QWidget* parent )
 * \brief return the new Applet Widget
 *
 * @param parent The parent of the Applet normally the taskbar
 */

/**
 * \fn int TaskbarAppletInterface::position()const;
 * \brief the wished position
 *
 * From left to right. 0 is left. The clock uses 10
 */
