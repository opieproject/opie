#include "okeyconfigmanager.h"

#include "okeyconfigmanager_p.h"

namespace Opie {
namespace Core {
namespace Internal {
    /*
     * the virtual and hardware key events have both issues...
     */
    void fixupKeys( int& key, int &mod, QKeyEvent* e ) {
        key = e->key();
        mod = e->state();
       /*
        * virtual keyboard
        * else change the button mod only
        */
        if ( key == 0 ) {
            key = e->ascii();
            if (  key > 96 && key < 123)
                key -=  32;
        }else{
            int new_mod = 0;
            if ( mod & 256 )
                new_mod |= Qt::ShiftButton;
            else if ( mod & 512 )
                new_mod |= Qt::ControlButton;
            else if ( mod & 1024 )
                new_mod |= Qt::AltButton;

            mod = new_mod == 0? mod : new_mod;
        }
    }
}

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
    m_key = key;
}

/**
 * Set the modifier key
 *
 * @param the Modifier key
 * @see Qt::ButtonState
 * @see modifier()
 */
void OKeyPair::setModifier( int mod ) {
    m_mod = mod;
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
    return OKeyPair();
}

/**
 * This functions uses the Opie::Core::ODevice::buttons
 * for OKeyPairList
 *
 * @see Opie::Core::ODevice
 * @see Opie::Core::ODeviceButton
 * @see Opie::Core::ODevice::button
 */
OKeyPair::List OKeyPair::hardwareKeys() {
    const QValueList<Opie::Core::ODeviceButton> but = Opie::Core::ODevice::inst()->buttons();
    OKeyPair::List lst;

    for ( QValueList<Opie::Core::ODeviceButton>::ConstIterator it = but.begin();
          it != but.end(); ++it )
        lst.append( OKeyPair( (*it).keycode(), 0 ) );


    return lst;
}

/**
 * Equals operator. Check if two OKeyPairs have the same key and modifier
 * @see operator!=
 */
bool OKeyPair::operator==( const OKeyPair& pair)const {
    if ( m_key != pair.m_key ) return false;
    if ( m_mod != pair.m_mod ) return false;

    return true;
}

/**
 * Not equal operator. calls the equal operator internally
 */
bool OKeyPair::operator!=( const OKeyPair& pair)const  {
    return !(*this == pair);
}


/**
 * The normal Constructor  to create a OKeyConfigItem
 *
 * You can set the the key paramater of this item but if
 * you use this item with the OKeyConfigManager your setting
 * will be overwritten.
 * You can also specify a QObject and slot which sould get called
 * once this item is activated. This slot only works if you
 * use the OKeyConfigManager.
 * The actual Key is read by load()
 *
 * \code
 * void MySlot::create(){
 *    OKeyConfigItem item(tr("Delete"),"delete",Resource::loadPixmap("trash"),
 *                        123, OKeyPair(Qt::Key_D,Qt::ControlButton),
 *                        this,SLOT(slotDelete(QWidget*,QKeyEvent*)));
 * }
 * \endcode
 *
 * @param text The text exposed to the user
 * @param config_key The key used in the config
 * @param pix A Pixmap associated  with this Item
 * @param def The OKeyPair used as default
 * @param caller The object where the slot exists
 * @param slot The slot which should get called
 *
 */
OKeyConfigItem::OKeyConfigItem( const QString& text, const QCString& config_key,
                                const QPixmap& pix, int id, const OKeyPair& def,
                                QObject *caller,
                                const char* slot )
    : m_text( text ), m_config( config_key ), m_pix( pix ),
      m_id( id ), m_def( def ),
      m_obj( caller ), m_str( slot ) {}

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
OKeyConfigItem::OKeyConfigItem( const Opie::Core::ODeviceButton& b )
    : m_text( b.userText() ), m_pix( b.pixmap() ), m_id( -1 ),
      m_key( OKeyPair( b.keycode(), 0 ) ), m_def( OKeyPair( b.keycode(), 0 ) )
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
 * @internal
 */
QObject* OKeyConfigItem::object()const{
    return m_obj;
}

/**
 * @internal
 */
QCString OKeyConfigItem::slot()const {
    return m_str;
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
    if ( m_id != -1       ) return false;

    return true;
}

/**
 * Check if the KeyPairs are the same
 */
bool OKeyConfigItem::operator==( const OKeyConfigItem& conf )const {
/*    if ( isEmpty() == conf.isEmpty() ) return true;
    else if ( isEmpty() != conf.isEmpty() ) return false;
    else if ( !isEmpty()!= conf.isEmpty() ) return false;
*/

    if ( m_id != conf.m_id )     return false;
    if ( m_obj != conf.m_obj )   return false;
    if ( m_text != conf.m_text ) return false;
    if ( m_key != conf.m_key )   return false;
    if ( m_def != conf.m_def )   return false;



    return true;

}

bool OKeyConfigItem::operator!=( const OKeyConfigItem& conf )const {
    return !( *this == conf );
}

/*! \enum OKeyConfigManager::EventMask
     <a name="Eventmask flags"></a>
     This enum is used to tell OKeyConfigManager which type of key events should inspected.

     <ul>
         <li>\c MaskPressed When a key is pressed an action performs
         <li>\c MaskReleased When a key is released an action performs
     </ul>
*/

/**
 * \brief c'tor
 * The Constructor for a OKeyConfigManager
 *
 * You can use this manager in multiple ways. Either make it handle
 * QKeyEvents. The EventMask is set to OKeyConfigManager::MaskReleased by default.
 *
 * \code
 * Opie::Core::Config conf = oApp->config();
 * Opie::Core::OKeyPairList blackList;
 * blackList.append(Opie::Core::OKeyPair::leftArrowKey());
 * blackList.append(Opie::Core::OKeyPair::rightArrowKey());
 * Opie::Core::OKeyConfigManager *manager = new Opie::Core::OKeyConfigManager(conf,"key_actions",blackList,
 *                                                                        false);
 * QListView *view = new QListView();
 * manager->handleWidget(view);
 * manager->addKeyConfig( Opie::Core::OKeyPair::returnKey());
 * manager->load();
 *
 * connect(manager,SIGNAL(actionActivated(QWidget*,QKeyEvent*,const Opie::Core::OKeyConfigItem&)),
 *         this,SLOT(slotHandleKey(QWidget*,QKeyEvent*,const Opie::Core::OKeyConfigItem&)));
 *
 * ....
 *
 * void update(){
 *  QDialog diag(true);
 *  QHBoxLayout *lay = new QHBoxLayout(&diag);
 *  Opie::Ui::OKeyConfigWidget *wid = new Opie::Ui::OKeyConfigWidget(manager,&diag);
 *  wid->setChangeMode(Opie::Ui::OKeyConfigWidget::Queu);
 *  lay->addWidget(wid);
 *  if(QPEApplication::execDialog( &diag)== QDialog::Accepted){
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
                                      const OKeyPair::List& black,
                                      bool grabkeyboard, QObject* par,
                                      const char* name)
    : QObject( par, name ), m_conf( conf ), m_group( group ),
      m_blackKeys( black ), m_grab( grabkeyboard ), m_map( 0 ){
    if ( m_grab )
        QPEApplication::grabKeyboard();
    m_event_mask = OKeyConfigManager::MaskReleased;
}


/**
 * Destructor
 */
OKeyConfigManager::~OKeyConfigManager() {
    if ( m_grab )
        QPEApplication::ungrabKeyboard();
    delete m_map;
}

/**
 * Load the Configuration  from the OConfig
 * If a Key is restricted but was in the config we will
 * make it be the empty key paur
 * We will change the group but restore to the previous.
 *
 * @see OKeyPair::emptyKey
 */
void OKeyConfigManager::load() {
    Opie::Core::OConfigGroupSaver grp( m_conf, m_group );

    /*
     * Read each item
     */
    int key, mod;
    for( OKeyConfigItem::List::Iterator it = m_keys.begin(); it != m_keys.end(); ++it ) {
        key = m_conf->readNumEntry( (*it).configKey()+"key",
                                    (*it).defaultKeyPair().keycode()  );
        mod = m_conf->readNumEntry( (*it).configKey()+"mod",
                                    (*it).defaultKeyPair().modifier() );
        OKeyPair okey( key, mod );

        if (  !m_blackKeys.contains( okey ) && key != -1  && mod != -1 )
            (*it).setKeyPair( okey );
        else
            (*it).setKeyPair( OKeyPair::emptyKey() );
    }
    delete m_map; m_map = 0;
}

/**
 * We will save the current configuration
 * to the OConfig. We will change the group but restore
 * to the previous
 */
void OKeyConfigManager::save() {
    Opie::Core::OConfigGroupSaver grp( m_conf, m_group );

    /*
     * Write each item
     */
    for( OKeyConfigItem::List::Iterator it = m_keys.begin();it != m_keys.end(); ++it ) {
        /* skip empty items */
        if ( (*it).isEmpty() )
            continue;
        OKeyPair pair = (*it).keyPair();
        OKeyPair deft = (*it).defaultKeyPair();
        /*
         * don't write if it is the default setting
         * FIXME allow to remove Keys from config
        if (  (pair.keycode() == deft.keycode()) &&
              (pair.modifier()== deft.modifier() ) )
            return;
        */

        m_conf->writeEntry((*it).configKey()+"key", pair.keycode()  );
        m_conf->writeEntry((*it).configKey()+"mod", pair.modifier() );
    }
    m_conf->write();
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
   /*
    * Fix Up issues with Qt/E, my keybard, and virtual input
    * methods
    * First my Keyboard delivers 256,512,1024 for shift/ctrl/alt instead of the button state
    * Also key() on virtual inputmethods are zero and only ascii. We need to fix upper and lower
    * case ascii
    */
    int key, mod;
    Opie::Core::Internal::fixupKeys( key, mod, e );

    OKeyConfigItem::List _keyList =  keyList( key );
    if ( _keyList.isEmpty() )
        return OKeyConfigItem();

    OKeyConfigItem item;
    for ( OKeyConfigItem::List::Iterator it = _keyList.begin(); it != _keyList.end();
          ++it ) {
        if ( (*it).keyPair().modifier() == mod ) {
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
 *
 */
Opie::Core::OKeyConfigItem::List OKeyConfigManager::keyConfigList()const{
    return m_keys;
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
OKeyPair::List OKeyConfigManager::blackList()const {
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

    /*
     * check  if we care for the event
     */
    if ( (ev->type() != QEvent::KeyPress||!testEventMask(MaskPressed)) &&
        (ev->type() != QEvent::KeyRelease||!testEventMask(MaskReleased)) )
        return false;

    QKeyEvent *key = static_cast<QKeyEvent*>( ev );
    OKeyConfigItem item =  handleKeyEvent( key );

    if ( item.isEmpty() )
        return false;

    QWidget *wid = static_cast<QWidget*>( obj );

    if ( item.object() && !item.slot().isEmpty() ) {
        connect( this, SIGNAL( actionActivated(QWidget*, QKeyEvent*)),
                 item.object(), item.slot().data() );
        emit actionActivated(wid, key);
        disconnect( this, SIGNAL(actionActivated(QWidget*,QKeyEvent*)),
                    item.object(), item.slot().data() );
    }
    emit actionActivated( wid, key, item );

    return true;
}

/**
 * @internal
 */
OKeyConfigItem::List OKeyConfigManager::keyList( int keycode) {
   /*
    * Create the map if not existing anymore
    */
    if ( !m_map ) {
        m_map = new OKeyMapConfigPrivate;
        /* for every key */
        for ( OKeyConfigItem::List::Iterator it = m_keys.begin();
              it!= m_keys.end(); ++it ) {

            bool add = true;
            /* see if this key is blocked */
            OKeyPair pair = (*it).keyPair();
            for ( OKeyPair::List::Iterator pairIt = m_blackKeys.begin();
                  pairIt != m_blackKeys.end(); ++pairIt ) {
                if ( (*pairIt).keycode()  == pair.keycode() &&
                     (*pairIt).modifier() == pair.modifier() ) {
                    add = false;
                    break;
                }
            }
            /* check if we added it */
            if ( add )
                (*m_map)[pair.keycode()].append( *it );
        }
    }
    return (*m_map)[keycode];
}

}
}
