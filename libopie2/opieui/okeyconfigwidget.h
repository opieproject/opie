/*
 * Copyright (C) 2004
 * LGPL v2  zecke@handhelds.org
 */


#ifndef ODP_KEY_CONFIG_WIDGET_H
#define ODP_KEY_CONFIG_WIDGET_H

#include <opie2/oconfig.h>
#include <opie2/odevice.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qbytearray.h>
#include <qhbox.h>
#include <qvaluelist.h>

class QKeyEvent;

namespace Opie {
namespace Ui {


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
    typedef QValueList<OKeyPair> OKeyPairList;
    OKeyPair( int key = -1, int modifier = -1);
    ~OKeyPair();

    bool operator==( const OKeyPair& );
    bool operator!=( const OKeyPair& );

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
    static OKeyPairList hardwareKeys();

private:
    int m_key = -1;
    int m_mod = -1;
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
    typedef QValueList<OKeyConfigItem> OKeyConfigItemList;
    OKeyConfigItem( const QString& text = QString::null , const QCString& config_key = QCString(),
                    const QPixmap& symbol  = QPixmap(),
                    int id = -1,
                    const OKeyPair& set = OKeyPair::emptyKey(),
                    const OKeyPair& def = OKeyPair::emptyKey() );
    OKeyConfigItem( const Opie::Core::ODeviceButton& );
    ~OKeyConfigItem();

    bool operator==( const OKeyConfigItem& );
    bool operator!=( const OKeyConfigItem& );

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
    void setId( int id );
    void setConfigKey( const QCString& );

private:
    int m_id;
    QString  m_text;
    QCString m_config;
    QPixmap  m_pix;
    OKeyPair m_key;
    OKeyPair m_def;
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
 *
 * @since 1.1.2
 */
class OKeyConfigManager : public QObject {
    Q_OBJECT
public:
    OKeyConfigManager(Opie::Core::OConfig *conf = 0,
                      const QString& group = QString::null,
                      OKeyPairList &block = OKeyPairList(),
                      bool grabkeyboard = false,  QObject *= 0,
                      const char* name = 0      );
    ~OKeyConfigManager();

    void load();
    void save();

    OKeyConfigItem handleKeyEvent( QKeyEvent* );
    QString        handleKeyEventString( QKeyEvent* );

    void addKeyConfig( const OKeyConfigItem& );
    void removeKeyConfig( const OKeyConfigItem& );

    void addBlackList( const OKeyPair& );
    void removeBlackList( const OKeyPair& );
    void clearBlackList();
    OKeyPairList blackList()const;

    void handleWidget( QWidget* );

    bool eventFilter( QObject*, QEvent* );
signals:
    void keyConfigChanged( Opie::Ui::OKeyConfigManager* );
    void actionActivated( QWidget*, QKeyEvent*, const Opie::Ui::OKeyConfigItem& );

private:
    OKeyPairList m_blackKeys;
    OKeyConfigItemList m_keys;
    QValueList<QWidget*> m_widgets;
    Opie::Core::OConfig *m_conf;
    QString m_group;
    bool m_grab : 1;
    class Private;
    Private *d;
};


class OKeyConfigWidget : public QHBox {
    Q_OBJECT
public:
    enum ChangeMode { Imediate, Queu };
    OKeyConfigWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    OKeyConfigWidget( OKeyConfigManager *, QWidget* parent = 0, const char* = 0, WFlags = 0 );
    ~OKeyConfigWidget();

    void setChangeMode( enum ChangeMode );
    ChangeMode changeMode()const;

    void setKeyConfig( OKeyConfigManager* );

    void reload();
    void save();
private:
    OKeyConfigManager* m_manager;
    class Private;
    Private *d;
};

}
}


#endif
