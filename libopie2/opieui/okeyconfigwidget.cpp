#include "okeyconfigwidget.h"




using namespace Opie::Ui;


/**
 * The default Constructor of a OKeyPair.
 * A Key and a Modifier ( Alt/Shift/Ctrl )
 * needs to be supplied.
 * Use Qt::Key for the information.
 * The default arguments  create an Empty OKeyPair. If you
 * want to get an empty OKeyPair use the static method for getting
 * the emptyKey()
 *
 * @see OKeyPair OKeyPair::emptyKey()
 */
OKeyPair::OKeyPair( int key, int mod )
    : m_key( key ), m_mod( mod )
{}

/**
 * The destructor
 */
OKeyPair::~OKeyPair() {}


/**
 * Is this OKeyPair empty/valid?
 */
bool OKeyPair::isEmpty()const {
    return ( ( m_key == -1 )&& ( m_mod == -1 ) );
}

/**
 * get the keycode for this OKeyPair. The Key relates to Qt::Key.
 *
 * @see Qt::Key
 * @see setKey
 */
int OKeyPair::keycode()const {
    return m_key;
}

/**
 * get the modifier key for this OKeyPair. The Modifier State relates
 * to the Qt::ButtonState
 *
 * @see Qt::ButtonState
 * @see setModifier
 */
int OKeyPair::modifier()const {
    return m_mod;
}


/**
 * Set the keycode
 * @param key The Keycode to set
 *
 * @see keycode()
 * @see Qt::Key
 */
void OKeyPair::setKeycode( int key ) {

}

/**
 * Set the modifier key
 *
 * @param the Modifier key
 * @see Qt::ButtonState
 * @see modifier()
 */
void OKeyPair::setModifier( int mod ) {

}

/**
 * Return  an OKeyPair for the Return Key without any modifier.
 */
OKeyPair OKeyPair::returnKey() {
    return OKeyPair( Qt::Key_Return, 0 );
}

/**
 * Return an OKeyPair for the Left Arrow Key
 * without any modifier Key
 */
OKeyPair OKeyPair::leftArrowKey() {
    return OKeyPair( Qt::Key_Left, 0 );
}

/**
 * Return an OKeyPair for the Right Arrow Key
 * without any modifier Key
 */
OKeyPair OKeyPair::rightArrowKey() {
    return OKeyPair( Qt::Key_Right, 0 );
}

/**
 * Return an OKeyPair for the Up Arrow Key
 * without any modifier Key
 */
OKeyPair OKeyPair::upArrowKey() {
    return OKeyPair( Qt::Key_Up, 0 );
}

/**
 * Return an OKeyPair for the Down Arrow Key
 * without any modifier Key
 */
OKeyPair OKeyPair::downArrowKey() {
    return OKeyPair( Qt::Key_Down, 0 );
}

/**
 * Return an Empty OKeyPair
 */
OKeyPair OKeyPair::emptyKey() {
    return OKeyPair;
}

/**
 * This functions uses the Opie::Core::ODevice::buttons
 * for OKeyPairList
 *
 * @see Opie::Core::ODevice
 * @see Opie::Core::ODeviceButton
 * @see Opie::Core::ODevice::button
 */
OKeyPairList OKeyPair::hardwareKeys() {
    const QValueList<Opie::Core::ODeviceButton> but = Opie::Core::ODevice::inst()->buttons();
    OKeyPairList lst;

    for ( QValueList<Opie::Core::ODeviceButton>::Iterator it = but.begin();
          it != but.end(); ++it )
        lst.append( OKeyPair( (*it).keycode(), 0 ) );


    return lst;
}

/**
 * Equals operator. Check if two OKeyPairs have the same key and modifier
 * @see operator!=
 */
bool OKeyPair::operator==( const OKeyPair& pair) {
    if ( m_key != pair.m_key ) return false;
    if ( m_mod != pair.m_mod ) return false;

    return true;
}

/**
 * Not equal operator. calls the equal operator internally
 */
bool OKeyPair::operator!=( const OKeyPair& pair) {
    return !(*this == pair);
}


/**
 * The normal Constructor  to create a OKeyConfigItem
 *
 * You can set the the key paramater of this item but if
 * you use this item with the OKeyConfigManager your setting
 * will be overwritten.
 *
 * @param text The text exposed to the user
 * @param config_key The key used in the config
 * @param pix A Pixmap associated  with this Item
 * @param key The OKeyPair used
 * @param def The OKeyPair used as default
 *
 */
OKeyConfigItem::OKeyConfigItem( const QString& text, const QCString& config_key,
                                const QPixmap& pix, int id, const OKeyPair& def,
                                const OKeyPair& key)
    : m_text( text ), m_config( config_key ), m_pix( pix ),
      m_id( id ), m_def( def ), m_key( key ) {}

/**
 * A special Constructor for converting from an Opie::Core::ODeviceButton
 * delivered by Opie::Core::ODevice::buttons()
 * There is no Config Key set and both default key and key are set
 * to Opie::Core::ODeviceButton::keycode() and 0 to modifier
 *
 * @see Opie::Core::ODevice
 * @see Opie::Core::ODeviceButton
 * @see Opie::Core::ODevice::buttons()
 */
OKeyConfigItem::OKeyConfigItem( Opie::Core::ODeviceButton& b )
    : m_text( b.userText() ), m_pix( b.pixmap() ), m_id( -1 )
      m_def( OKeyPair( b.keycode(), 0 ) ), m_key( OKeyPair( b.keycode(), 0 ) )
{}


/**
 * Destructor
 */
OKeyConfigItem::~OKeyConfigItem() {}


/**
 * The text exposed to the user
 *
 * @see setText
 */
QString OKeyConfigItem::text()const {
    return m_text;
}

/**
 * The pixmap shown to the user for your action/key
 *
 * @see setPixmap
 */
QPixmap OKeyConfigItem::pixmap()const {
    return m_pix;
}

/**
 * Return the OKeyPair this OKeyConfigItem is configured for.
 *
 * @see setKeyPair
 */
OKeyPair OKeyConfigItem::keyPair()const {
    return m_key;
}

/**
 * Return the default OKeyPair
 * @see setDefaultKeyPair
 */
OKeyPair OKeyConfigItem::defaultKeyPair()const {
    return m_def;
}


/**
 * Return the Id you assigned to this item.
 * setting is only possible by the constructor
 */
int OKeyConfigItem::id()const{
    return m_id;
}

/**
 * reutrn the Config Key. Setting it is only possible
 * by the constructor
 */
QCString OKeyConfigItem::configKey()const {
    return m_config;
}

/**
 * Set the text
 *
 * @param text Set the Text of this Action to text
 * @see text()
 */
void OKeyConfigItem::setText( const QString& text ) {
    m_text = text;
}

/**
 * Set the pixmap of this action
 *
 * @param pix The Pixmap to set
 * @see pixmap()
 */
void OKeyConfigItem::setPixmap( const QPixmap& pix ) {
    m_pix = pix;
}

/**
 * Set the KeyPair the OKeyConfigItem uses.
 * Your set Key could get overwritten if you use
 * the manager or GUI to configure the key
 *
 * @param key Set the OKeyPair used
 * @see keyPair()
 */
void OKeyConfigItem::setKeyPair( const OKeyPair& key) {
    m_key = key;
}

/**
 * Set the default KeyPair.
 *
 * @param key The default keypair
 * @see defaultKeyPair()
 */
void OKeyConfigItem::setDefaultKeyPair( const OKeyPair& key ) {
    m_def = key;
}

/**
 * @internal
 */
void OKeyConfigItem::setConfigKey( const QCString& str) {
    m_config = str;
    m_config.detach();
}

/**
 * @internal
 */
void OKeyConfigItem::setId( int id ) {
    m_id = id;
}

/**
 * If the item is not configured isEmpty() will return true
 * It is empty if no text is present and no default
 * and no configured key
 */
bool OKeyConfigItem::isEmpty()const {
    if ( !m_def.isEmpty()  ) return false;
    if ( !m_key.isEmpty()  ) return false;
    if ( !m_text.isEmpty() ) return false;

    return true;
}

/**
 * Check if the KeyPairs are the same
 */
bool OKeyConfigItem::operator==( const OKeyConfigItem& conf ) {
    if ( isEmpty() == conf.isEmpty() ) return true;
    else if ( isEmpty() != conf.isEmpty() ) return false;
    else if ( !isEmpty()!= conf.isEmpty() ) return false;

    if ( m_id != conf.m_id )     return false;
    if ( m_text != conf.m_text ) return false;
    if ( m_pix != conf.m_pix )   return false;
    if ( m_key != conf.m_key )   return false;
    if ( m_def != conf.m_def )   return false;

    return true;

}

bool OKeyConfigItem::operator!=( const OKeyConfigItem& conf ) {
    return !( *this == conf );
}

/**
 * \brief c'tor
 * The Constructor for a OKeyConfigManager
 *
 * You can use this manager in multiple ways. Either make it handle
 * QKeyEvents
 *
 * \code
 * Opie::Core::Config conf = oApp->config();
 * Opie::Ui::OKeyPairList blackList;
 * blackList.append(Opie::Ui::OKeyPair::leftArrowKey());
 * blackList.append(Opie::Ui::OKeyPair::rightArrowKey());
 * Opie::Ui::OKeyConfigManager *manager = new Opie::Ui::OKeyConfigManager(conf,"key_actions",blackList,
 *                                                                        false);
 * QListView *view = new QListView();
 * manager->handleWidget(view);
 * manager->addKeyConfig( Opie::Ui::OKeyPair::returnKey());
 * manager->load();
 *
 * connect(manager,SIGNAL(actionActivated(QWidget*,QKeyEvent*,const Opie::Ui::OKeyConfigItem&)),
 *         this,SLOT(slotHandleKey(QWidget*,QKeyEvent*,const Opie::Ui::OKeyConfigItem&)));
 *
 * ....
 *
 * void update(){
 *  QDialog diag(true);
 *  QHBoxLayout *lay = new QHBoxLayout(&diag);
 *  Opie::Ui::OKeyConfigWidget *wid = new Opie::Ui::OKeyConfigWidget(manager,&diag);
 *  wid->setChangeMode(Opie::Ui::OKeyConfigWidget::Queu);
 *  lay->addWidget(wid);
 *  if(QPEApplication::execDialog( &diag)== QDialog::Accept){
 *        wid->save();
 *  }
 * }
 *
 * ....
 * MyListView::keyPressEvent( QKeyEvent* e ){
 *     Opie::Ui::OKeyConfigItem item = manager->handleKeyEvent(e);
 *     if(!item.isEmpty() ){
 *         switch(item.id()){
 *           case My_Delete_Key:
 *             break;
 *         }
 *     }
 * }
 *
 * \endcode
 *
 * @param conf The Config where the KeySetting should be stored
 * @param group The group where the KeySetting will be stored
 * @param black Which keys shouldn't be allowed to handle
 * @param grabkeyboard Calls QPEApplication::grabKeyboard to allow handling of DeviceButtons
 * @param par The parent/owner of this manager
 * @param name The name of this object
 */
OKeyConfigManager::OKeyConfigManager( Opie::Core::OConfig* conf,
                                      const QString& group,
                                      OKeyPairList black,
                                      bool grabkeyboard, QObject* par,
                                      const char* name)
    : QObject( par, name ), m_conf( conf ), m_group( group ),
      m_blackKeys( black ), m_grab( grabkeyboard ), m_map( 0 )
{}


/**
 * Destructor
 */
OKeyConfigManager::~OKeyConfigManager() {}

/**
 * Load the Configuration  from the OConfig
 * If a Key is restricted but was in the config we will
 * make it be the empty key paur
 * We will change the group of the OConfig Item!
 *
 * @see OKeyPair::emptyKey
 */
void OKeyConfigWidget::load() {
    m_conf->setGroup( m_group );

    /*
     * Read each item
     */
    int key, mod;
    for( OKeyConfigItemList::Iterator it = m_keys.begin();
         it != m_keys.end(); ++it ) {
        key = m_conf->readNumEntry( (*it).configKey()+"key", (*it).defaultKeyPair().keycode()  );
        mod = m_conf->readNumEntry( (*it).configKey()+"mod", (*it).defaultKeyPair().modifier() );
        OKeyPair okey( key, mod );
        if (  !m_blackKeys.contains( okey ) && key != -1  && mod != -1 )
            (*it).setKeyPair( OKeyPair(key, mod) );
        else
            (*it).setKeyPair( OKeyPair::emptyKey() );
    }
    delete m_map; m_map = 0;
}

/**
 * We will save the current configuration
 * to the OConfig. We will change the group.
 */
void OKeyConfigWidget::save() {
    m_conf->setGroup( m_group );

    /*
     * Write each item
     */
    int key, mod;
    for( OKeyConfigItemList::Iterator it = m_keys.begin();
         it != m_keys.end(); ++it ) {
        if ( (*it).isEmpty() )
            continue;
        OKeyPair pair = (*it).keyPair();
        m_conf->writeEntry(pair.configKey()+"key", pair.keycode()  );
        m_conf->writeEntry(pair.configKey()+"mod", pair.modifier() );
    }
}

/**
 * This is function uses a QMap internally  but you can have the same keycode
 * with different modifier key. The behaviour is undefined if you add a OKeyConfigItem
 * with same keycode and modifier key. The GUI takes care that a user can't
 * cofigure two keys.
 *
 * Make sure you call e->ignore if you don't want to handle this event
 */
OKeyConfigItem OKeyConfigManager::handleKeyEvent( QKeyEvent* e ) {
    OKeyConfigItemList keyList =  keyList( e->key() );
    if ( keyList.isEmpty() )
        return OKeyConfigItem();

    OKeyConfigItem item;
    for ( OKeyConfigItemList::Iterator it = keyList.begin(); it != keyList.end();
          ++it ) {
        if ( (*it).keyPair().modifier() == e->state() ) {
            item = *it;
            break;
        }

    }

    return item;
}

/**
 * Return the associated id of the item or -1 if no item
 * matched the key
 *
 * @see handleKeyEvent
 */
int OKeyConfigManager::handleKeyEventId( QKeyEvent* ev) {
    return handleKeyEvent( ev ).id();
}

/**
 * Add Key Config to the List of items
 */
void OKeyConfigManager::addKeyConfig( const OKeyConfigItem& item ) {
    m_keys.append( item );
    delete m_map; m_map = 0;
}

/**
 * Remove the Key from the Config. Internal lists will be destroyed
 * and rebuild on demand later
 */
void OKeyConfigManager::removeKeyConfig( const OKeyConfigItem& item ) {
    m_keys.remove( item );
    delete m_map; m_map = 0;
}

/**
 * Clears the complete list
 */
void OKeyConfigManager::clearKeyConfig() {
    m_keys.clear();
    delete m_map; m_map = 0;
}


/**
 * Add this OKeyPair to the blackList.
 * Internal lists will be destroyed
 */
void OKeyConfigManager::addToBlackList( const OKeyPair& key) {
    m_blackKeys.append( key );
    delete m_map; m_map = 0;
}


/**
 * Remove this OKeyPair from the black List
 * Internal lists will be destroyed
 */
void OKeyConfigManager::removeFromBlackList( const OKeyPair& key ) {
    m_blackKeys.remove( key );
    delete m_map; m_map = 0;
}


/**
 * Clear the blackList
 */
void OKeyConfigManager::clearBlackList() {
    m_blackKeys.clear();
    delete m_map; m_map = 0;
}


/**
 * Return a copy of the blackList
 */
OKeyPairList OKeyConfigManager::blackList()const {
    return m_blackKeys;
}


/**
 * Ask the Manager to handle KeyEvents for you.
 * All handled keys will emit a QSignal and return true
 * that it handled the keyevent
 */
void OKeyConfigManager::handleWidget( QWidget* wid ) {
    wid->installEventFilter( this );
}

/**
 * @internal
 */
bool OKeyConfigManager::eventFilter( QObject* obj, QEvent* ev) {
    if ( !obj->isWidgetType() )
        return false;

    if ( ev->type() != QEvent::KeyPress && ev->type() != QEvent::KeyRelease )
        return false;

    QKeyEvent *key = static_cast<QKeyEvent*>( ev );
    OKeyConfigItem item =  handleKeyEvent( key );

    if ( item.isEmpty() )
        return false;

    emit actionActivated( static_cast<QWidget*>( obj ), key, item );
    return true;
}

/**
 * @internal
 */
OKeyConfigItemList OKeyConfigManager::keyList( int keycode) {
    if ( !m_map ) {
        m_map = new OKeyMapConfigPrivate;
        /* for every key */
        for ( OKeyConfigItemList::Iterator it = m_keys.begin();
              it!= m_keys.end(); ++it ) {
            bool add = true;
            /* see if this key is blocked */
            for ( OKeyPairList::Iterator pairIt = m_blackKeys.begin();
                  pairIt != m_blackKeys.end(); ++pairIt ) {
                if ( (*pairIt).keycode()  == (*it).keycode() &&
                     (*pairIt).modifier() == (*it).modifier() ) {
                    add = false;
                    break;
                }
            }
            /* check if we added it */
            if ( add ) {
                if ( m_map->contains( (*it).keycode() ) )
                    (m_map[(*it).keycode()]).append( *it );
                else
                    m_map.insert( (*it).keycode(), OKeyConfigItemList( *it ) );
            }
        }
    }
    return m_map[keycode];
}



/////////////////////////
//////// Widget Starts Here
namespace Opie {
namespace Ui {
namespace Private {
    static QString keyToString( const OKeyPair& );
    class OItemBox : public QHBox {
        Q_OBJECT
    public:
        OItemBox( const OKeyConfigItem& item, QWidget* parent = 0, const char* name = 0,  WFlags fl = 0);
        ~OItemBox();

        OKeyConfigItem item()const;
        void setItem( const OKeyConfigItem& item );
    private slots:
        void slotClicked();
    signals:
        void configureBox( OItemBox* );
    private:
        QLabel *m_pix;
        QLabel *m_text;
        QPushButton *m_btn;
        OKeyConfigItem m_item;
    };

    OItemBox::OItemBox( const OKeyConfigItem& item, QWidget* parent,
                        const char* name, WFlags fl )
        : QHBox( parent, name, fl ),  {
        m_pix  = new QLabel( this );
        m_text = new QLabel( this );
        m_btn  = new QPushButton( this );

        connect(m_btn, SIGNAL(clicked()),
                this, SLOT(slotClicked()));

        setItem( item );
    }

    OItemBox::~OItemBox() {}
    OKeyConfigItem OItemBox::item()const{
        return m_item;
    }
    void OKeyConfigItem::setItem( const OKeyConfigItem& item ) {
        m_item = item;
        m_pix ->setPixmap( item.pixmap() );
        m_text->setText( item.text() );
        m_btn->setText( keyToString( item.keyPair() ) );
    }
    void OKeyConfigItem::slotClicked() {
        emit configureBox( this );
    }

    QString keyToString( const OKeyPair& pair ) {
        QStringList mod;
        if ( ( pair.modifier() & Qt::ShiftButton )== Qt::ShiftButton )
            mod.append( QObject::tr( "Shift", "The Keyboard key" ) );
        if ( ( pair.modifier() & Qt::ControlButton )== Qt::ControlButton )
            mod.append( QObject::tr( "Ctrl", "The Ctrl key" ) );
        if ( ( pair.modifier() & Qt::AltButton ) )== Qt::AltButton )
            mod.append( QObject::tr( "Alt", "The keyboard Alt Key" ) );


    }
}
}
}



////////////////////////



#include "okeyconfigwidget.moc"
