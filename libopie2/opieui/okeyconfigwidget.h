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
#include <qcstring.h>
#include <qhbox.h>
#include <qvaluelist.h>

class QKeyEvent;
class QLabel;
class QPushButton;
class QListViewItem;
class QRadioButton;

namespace Opie {
namespace Ui {
namespace Private {
    class OKeyConfigWidgetPrivate;
    typedef QValueList<OKeyConfigWidgetPrivate> OKeyConfigWidgetPrivateList;
}
    class OListViewItem;
    class OListView;

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
 * You need to load ans save yourself!
 *
 * @since 1.1.2
 */
class OKeyConfigManager : public QObject {
    Q_OBJECT
    typedef QMap<int, OKeyConfigItem::List> OKeyMapConfigPrivate;
public:
    OKeyConfigManager(Opie::Core::OConfig *conf = 0,
                      const QString& group = QString::null,
                      const OKeyPair::List &block = OKeyPair::List(),
                      bool grabkeyboard = false,  QObject * par = 0,
                      const char* name = 0      );
    ~OKeyConfigManager();

    void load();
    void save();

    OKeyConfigItem handleKeyEvent( QKeyEvent* );
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
signals:
    /**
     * The Signals are triggered on KeyPress and KeyRelease!
     * You can check the isDown of the QKeyEvent
     * @see QKeyEvent
     */
    void actionActivated( QWidget*, QKeyEvent*, const Opie::Ui::OKeyConfigItem& );

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
};


/**
 * With this Widget you can let the Keyboard Shortcuts
 * be configured by the user.
 * There are two ways you can use this widget. Either in a tab were
 * all changes are immediately getting into effect or in a queue
 * were you ask for saving. Save won't write the data but only set
 * it to the OKeyConfigManager
 *
 * @since 1.2
 */
class OKeyConfigWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * Immediate Apply the change directly to the underlying OKeyConfigManager
     * Queue Save all items and then apply when you save()
     */
    enum ChangeMode { Imediate, Queue };
    OKeyConfigWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~OKeyConfigWidget();

    void setChangeMode( enum ChangeMode );
    ChangeMode changeMode()const;

    void insert( const QString& name, OKeyConfigManager* );

    void load();
    void save();

private slots:
    void slotListViewItem(  QListViewItem* );
    void slotNoKey();
    void slotDefaultKey();
    void slotCustomKey();


private:
    void initUi();
    Opie::Ui::OListView *m_view;
    Opie::Ui::Private::OKeyConfigWidgetPrivateList m_list;
    QLabel *m_lbl;
    QPushButton *m_btn;
    QRadioButton *m_def, *m_cus, *m_none;
    QWidget* m_box;
    ChangeMode m_mode;
    class Private;
    Private *d;
};

}
}


#endif
