/*
 * Copyright (C) 2004
 * LGPL v2  zecke@handhelds.org
 */


#ifndef ODP_KEY_MANAGER_WIDGET_H
#define ODP_KEY_MANAGER_WIDGET_H

#include <opie2/oconfig.h>
#include <opie2/odevice.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qcstring.h>
#include <qvaluelist.h>

namespace Opie {
namespace Core {

/**
 * \brief small class representing a Key with possible modifiers
 * This class holds information about key code and possible
 * modifier state. That is the lowest level of the key input
 * functions.
 * There are also static methods to get special keys.
 * OKeyPair will be used with \see OKeyConfigItem
 *
 * @since 1.2
 */
class OKeyPair {
public:
    typedef QValueList<OKeyPair> List;
    OKeyPair( int key = -1, int modifier = -1);
    ~OKeyPair();

    bool operator==( const OKeyPair& )const;
    bool operator!=( const OKeyPair& )const;

    bool isEmpty()const;

    int keycode()const;
    int modifier()const;

    void setKeycode( int );
    void setModifier( int );

    static OKeyPair returnKey();
    static OKeyPair leftArrowKey();
    static OKeyPair rightArrowKey();
    static OKeyPair upArrowKey();
    static OKeyPair downArrowKey();
    static OKeyPair emptyKey();
    static OKeyPair::List hardwareKeys();

private:
    int m_key;
    int m_mod;
    class Private;
    Private* d;
};

/**
 * A class to represent an OKeyPair.
 * It consists out of a Text exposed to the user, Config Key Item,
 * Pixmap, A default OKeyPair and the set OKeyPair.
 * You can also pass an id to it
 *
 * @since 1.1.2
 */
class OKeyConfigItem {
    friend class OKeyConfigManager;
public:
    typedef QValueList<OKeyConfigItem> List;
    OKeyConfigItem( const QString& text = QString::null , const QCString& config_key = QCString(),
                    const QPixmap& symbol  = QPixmap(),
                    int id = -1,
                    const OKeyPair& def = OKeyPair::emptyKey(),
                    QObject *caller = 0, const char* slot = 0);
    OKeyConfigItem( const Opie::Core::ODeviceButton& );
    ~OKeyConfigItem();

    bool operator==( const OKeyConfigItem& )const;
    bool operator!=( const OKeyConfigItem& )const;

    QString text()const;
    QPixmap pixmap()const;
    int id()const;



    OKeyPair keyPair()const;
    OKeyPair defaultKeyPair()const;
    QCString configKey()const;


    void setText( const QString& text );
    void setPixmap( const QPixmap& );
    void setKeyPair( const OKeyPair& );
    void setDefaultKeyPair( const OKeyPair& );

    bool isEmpty()const;

protected:
    QObject *object()const;
    QCString slot()const;
    void setId( int id );
    void setConfigKey( const QCString& );

private:
    QString  m_text;
    QCString m_config;
    QPixmap  m_pix;
    int m_id;
    OKeyPair m_key;
    OKeyPair m_def;
    QObject *m_obj;
    QCString m_str;
    class Private;
    Private *d;
};



/**
 * \brief A manager  to load and save Key Actions and get notified
 * This is the Manager for KeyActions.
 * You can say from which config and group to read data, to grab the
 * keyboard to handle hardware keys, you can supply a blacklist of
 * keys which should not be used by allowed to be used.
 * You can even pass this manager to a Widget to do the configuration for you.
 * You need to add OKeyConfigItem for your keys and then issue a load() to
 * read the Key information.
 * You can either handle the QKeyEvent yourself and ask this class if it is
 * handled by your action and let give you the action. Or you can install
 * the event filter and get a signal.
 * You need to load and save yourself!
 *
 * Again if you want to extend it and I missed a virtual, tell me so I can improve (zecke@handhelds.org)
 *
 * @since 1.1.2
 */
class OKeyConfigManager : public QObject {
    Q_OBJECT
    typedef QMap<int, OKeyConfigItem::List> OKeyMapConfigPrivate;
public:
    enum EventMask {
        MaskPressed = 0x1,
        MaskReleased = 0x2,
    };

    OKeyConfigManager(Opie::Core::OConfig *conf = 0,
                      const QString& group = QString::null,
                      const OKeyPair::List &block = OKeyPair::List(),
                      bool grabkeyboard = false, QObject * par = 0,
                      const char* name = 0      );
    virtual ~OKeyConfigManager();

    virtual void load();
    virtual void save();

    virtual OKeyConfigItem handleKeyEvent( QKeyEvent* );
    int            handleKeyEventId( QKeyEvent* );

    void addKeyConfig( const OKeyConfigItem& );
    void removeKeyConfig( const OKeyConfigItem& );
    void clearKeyConfig();

    void addToBlackList( const OKeyPair& );
    void removeFromBlackList( const OKeyPair& );
    void clearBlackList();
    OKeyPair::List blackList()const;

    void handleWidget( QWidget* );

    bool eventFilter( QObject*, QEvent* );

    /**
     * Sets the event mask flags aMask.
     *
     * aMask is a combination of OKeyConfigManager::EventMask
     *
     * @see eventMask(), testEventMask(), addEventMask(), clearEventMask()
     */
    void setEventMask(uint aMask);
    /**
     * Returns the event mask flags set.
     *
     * aMask is a combination of OKeyConfigManager::EventMask
     *
     * @see setEventMask(), testEventMask(), addEventMask(), clearEventMask()
     */
    uint eventMask()const;
    /**
     * Test if the event mask flag aMask is set.
     *
     * @param aMask one of OKeyConfigManager::EventMask
     *
     * @see eventMask(), setEventMask(), addEventMask(), clearEventMask()
     */
    bool testEventMask(uint aMask);
    /**
     * Add the event mask flag aMask.
     *
     * @param aMask one of OKeyConfigManager::EventMask
     *
     * @see eventMask(), setEventMask(), addEventMask(), clearEventMask()
     */
    void addEventMask(uint aMask);
    /**
     * Clears the event mask flag aMask.
     *
     * @param aMask is one of OKeyConfigManager::EventMask
     *
     * @see eventMask(), testEventMask(), addEventMask(), setEventMask()
     */
    void clearEventMask(uint aMask);

    OKeyConfigItem::List keyConfigList()const;

signals:
    /**
     * The Signals are triggered on KeyPress and KeyRelease!
     * You can check the isDown of the QKeyEvent
     * @see QKeyEvent
     */
    void actionActivated( QWidget*, QKeyEvent*, const Opie::Core::OKeyConfigItem& );

    /**
     * This Signal correspondents to the OKeyConfigItem slot
     * and object
     *
     * @see OKeyConfigItem::slot
     * @see OKeyConfigItem::object
     */
    void actionActivated( QWidget* par, QKeyEvent* key);

private:
    OKeyConfigItem::List keyList( int );
    OKeyConfigItem::List m_keys;
    QValueList<QWidget*> m_widgets;
    Opie::Core::OConfig *m_conf;
    QString m_group;
    OKeyPair::List m_blackKeys;
    bool m_grab : 1;
    OKeyMapConfigPrivate *m_map;
    class Private;
    Private *d;
    uint m_event_mask;
};

inline bool OKeyConfigManager::testEventMask(uint aMask)
{
    return (m_event_mask&aMask)!=0;
}

inline void OKeyConfigManager::addEventMask(uint aMask)
{
    m_event_mask |= aMask;
}

inline void OKeyConfigManager::clearEventMask(uint aMask)
{
    m_event_mask &= ~aMask;
}

inline void OKeyConfigManager::setEventMask(uint aMask)
{
    m_event_mask = aMask;
}

inline uint OKeyConfigManager::eventMask()const
{
    return  m_event_mask;
}

}
}

#endif
