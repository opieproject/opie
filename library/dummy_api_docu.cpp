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


/**
 * \class WindowDecorationInterface
 *
 * Interface class for Window Decorations. Yu need to implement
 * metric and drawing functions.
 */

/**
 * \class WindowDecorationInterface::WindowData
 *
 * Window informations like the QRect, Palette, Caption
 * and flag
 */

/**
 * \fn int WindowDecorationInterface::metric(Metric m,const WindowData* )
 *
 * Return the width for the item out of Metric.
 * Normally you will case Metric and default: should call the interface
 * method. Also return 0
 */

/**
 * \fn void WindowDecorationInterface::drawArea( Area a, QPainter* , const WindowData* )const
 *
 * draw the Area specefic in a to the QPainter
 */

/**
 * \fn void WindowDecorationInterface::drawButton(Button b,QPainter*p ,const WindowData* d, int x, int y, int w,int h, QWSButton::State s)const
 *
 * @param b The Button to be drawn
 * @param p The painter to draw at
 * @param d The Window Data
 * @param x The X position of the button
 * @param y The Y position of the button
 * @param w The width of the button
 * @param h The height of the button
 * @param s The state of the button
 */

/**
 * \fn QRegion WindowDecorationInterface::mask( const WindowData* )const
 *
 * The mask of the Decoration.
 *
 * \code
 *     int th = metric(TitleHeight,wd);
 *   QRect rect( wd->rect );
 *   QRect r(rect.left() - metric(LeftBorder,wd),
 *          rect.top() - th - metric(TopBorder,wd),
 *          rect.width() + metric(LeftBorder,wd) + metric(RightBorder,wd),
 *          rect.height() + th + metric(TopBorder,wd) + metric(BottomBorder,wd));
 *  return QRegion(r) - rect;
 * \endcode
 */


/**
 * \class DateFormat
 * \brief The format a Date and Time is composed
 *
 * Define how a Date and Time should be composed
 *
 * @see TimeString
 */

/**
 * \enum DateFormat::Order
 * The order of the Day,Month and Year
 */
/**
 * \val DateFormat::Order::DayMonthYear
 * First the Day, then the Month and the Year
 */

/**
 * \val DateFormat::Order::MonthDayYear
 * First the Month then Day and Year followed
 */
/**
 * \val DateFormat::Order::YearMonthDay
 * Year Month and then the Day
 */

/**
 * \fn QString DateFormat::toNumberString()const
 *  Return the Order of Days as string (M/D/Y) in example
 *  this string can be shown to the user.
 *  It will extract three bits at a time and compose
 *  a string
 */

/**
 * \fn QString DateFormat::toWordString()const
 *  Return the DateFormat as written words
 *
 */

/**
 * \fn DateFormat::DateFormat(QChar s,Order or)
 * A Constructor.
 * @param s The seperator for the short representation. As used by toNumberString
 * @param or The order used for short and long dates
 */

/**
 * \fn DateFormat::DateFormat(QChar s,Order so,Order lo)
 *
 * A different Constructor with different long and short representation
 * @param s The seperator
 * @param so The short order representation
 * @param lo The long order representation
 */


/**
 * \fn DateFormat::DateFormat(const DateFormat&)
 * Copy c'tor
 *
 */

/**
 * \enum DateFormat::Verbosity
 * The verbosity as used by DateFormat::numberDate() and DateFormat::wordDate()
 *
 * \enum val shortNumber Express the Year as 04
 * \enum val longNumber Express the Year as 2004
 * \enum val showWeekDay Use week day names
 * \enum val longWord Use long word. If not set the week day will be stripped to the first three characters
 *
 *
 * @see DateFormat::numberDate
 * @see DateFormat::wordDate
 */

/**
 * \fn QString DateFormat::numberDate(const QDate& d,int v)const
 *
 *  Compose the QDate to a QString using the shortOrder() and
 *  the verbosity. The verbosity is only used to see if the
 *  year should be four positions or two positions wide.
 *
 *  @param d The QDate to be expressed as string
 *  @param v Only DateFormat::Verbosity::shortNumber or
 *           DateFormat::Verbosity::longNumber is used.
 *           Use this to say if the year should be two or four
 *           positions wide
 *
 * @see DateFormat::wordDate()
 */

/**
 * \fn  QString DateFormat::wordDate(const QDate& d,int v )const
 * Compose the QDate to a QString using the longOrder() options.
 * The difference to numberDate() is the length of the resulting
 * string. Weekday- and Monthnames are expressed as words as opposed to digits.
 *
 *
 * @param d The Date to be used
 * @param v The verbosity
 */

/**
 * \fn void DateFormat::load(QDataStream&)
 * load from the QDateStream. And set the values
 * internally
 */

/**
 * \fn void DateFormat::save(QDateStream&)const
 * save this DateFormat into the QDataStream
 */

/**
 *\fn QChar DateFormat::seperator()const
 * The seperator used for the numberString()
 */
/**
 * \fn DateFormat::Order DateFormat::shortOrder()const
 * Return the DateFormat::Order for the shortOrder which will be used by numberString
 */
/**
 * \fn DateFormat::Order DateFormat::longOrder()const
 * Return the DateFormat::Order for the longOrder which will be used by the wordString
 */

/**
 * \class TimeString
 * \brief A frontend  to DateFormat
 *
 * A utility class to use DateFormat more easily
 * @todo For ODP add QDateTime and QTime  operations honoring system settings
 */

/**
 * \fn QString TimeString::shortDate(const QDate& d)
 * Call the overloaded function with currentDateFormat()
 *
 * @param d The QDate to be transformed
 * @see TimeString::shortDate(const QDate& d,DateFormat)
 */
/**
 * \fn QString TimeString::dateString(const QDate& d);
 * This function calls the overloaded function with currentDateFormat()
 *
 * @param d The QDate to be used
 * @see TimeString::dateString(const QDate& d,DateFormat);
 */
/**
 * \fn QString TimeString::longDateString(const QDate& d);
 *
 * This functions calls the olverloaded function with currentDateFormat()
 *
 * @param d The QDate to be used
 * @see TimeString::longDateString(const QDate& d,DateFormat);
 */
/**
 * \fn QString TimeString::dateString(const QDateTime&,bool,bool);
 * @internal
 */
/**
 * \fn QString TimeString::dateString(const QDateTime& t,bool)
 * Do not use they do not honor system wide AMPM settings.
 * Also
 *
 * @see numberDateString
 * @see longNumberDateString
 */
/**
 * \fn QString TimeString::numberDateString(const QDate& d, DateFormat f )
 *  Return the date as String in number coding (DD/MM/YY) depending on the
 *  date format
 *
 * @param d The QDate to be used
 * @param f The DateFormat to be used
 */
/**
 * \fn QString TimeString::numberDateString(const QDate& d)
 * This is an overloaded function and uses the currentDateFormat()
 * of the system
 *
 * @see currentDateFormat()
 * @param d The QDate to be used
 */
/**
 * \fn QString TimeString::longNumberDateString(const QDate& d, DateFormat f )
 *  Return the date as String in number coding (DD/MM/YYYY) depending on the
 *  date format. This uses the DateFormat::longNumber verbosity on the DateFormat::numberDate
 *  method
 *
 * @param d The QDate to be used
 * @param f The DateFormat to be used
 */
/**
 * \fn QString TimeString::longNumberDateString(const QDate& d)
 * This is an overloaded function and uses the currentDateFormat()
 * of the system.
 *
 *
 * @see currentDateFormat()
 * @see longNumberDateString(const QDate&,DateFormat)
 * @param d The QDate to be used
 */


/**
 * \fn QString TimeString::shortDate(const QDate& d,DateFormat f)
 * Calls DateFormat::wordDate with d
 *
 * Example: '20 Feb 04'
 *
 *
 *
 * @param d The QDate used
 * @param f DateFormat which will be used for composing
 * @todo Make naming consistent with the two other functions calling wordDate
 */

/**
 * \fn QString TimeString::dateString(const QDate& d,DateFormat f )
 * Calls and translates DateFormat::wordDate with d and
 * DateFormat::longNumber|DateFormat::longWord
 * This means we will translate the month name and have a 4 digit year (2004)
 *
 * Example: '20 Feb 2004'
 *
 * @param d The QDate.
 * @param f The DateFormat which will be called
 */

/**
 * \fn QSrring TimeString::longDateString(const QDate& d, DateFormat f )
 * Calls and translates DateFormat::wordDate with DateFormat::showWeekDay|DateFormat::longNumber|DateFormat::longWord.
 * This means you will have a string with the week day name, monthname and a four digit year.
 *
 * Example: 'Fr, 20 Feb 2004'
 *
 * @param d The QDate
 *
 */

/**
 * \fn DateFormat currentDateFormat()
 * Return the System DateFormat
 */
