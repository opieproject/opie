#include "okeyconfigwidget.h"
#include "okeyconfigwidget_p.h"


#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qaccel.h>
#include <qlayout.h>
#include <qlabel.h>

/* non gui */
#include <qtimer.h>


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
}


/**
 * Destructor
 */
OKeyConfigManager::~OKeyConfigManager() {
    if ( m_grab )
        QPEApplication::ungrabKeyboard();
}

/**
 * Load the Configuration  from the OConfig
 * If a Key is restricted but was in the config we will
 * make it be the empty key paur
 * We will change the group of the OConfig Item!
 *
 * @see OKeyPair::emptyKey
 */
void OKeyConfigManager::load() {
    m_conf->setGroup( m_group );

    /*
     * Read each item
     */
    int key, mod;
    for( OKeyConfigItem::List::Iterator it = m_keys.begin(); it != m_keys.end(); ++it ) {
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
void OKeyConfigManager::save() {
    m_conf->setGroup( m_group );

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
    Opie::Ui::Private::fixupKeys( key, mod, e );

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
Opie::Ui::OKeyConfigItem::List OKeyConfigManager::keyConfigList()const{
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

    if ( ev->type() != QEvent::KeyPress && ev->type() != QEvent::KeyRelease )
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


namespace Opie {
namespace Ui {
namespace Private {

    OKeyListViewItem::OKeyListViewItem( const OKeyConfigItem& item, OKeyConfigManager* man, OListViewItem* parent)
        : Opie::Ui::OListViewItem( parent ), m_manager( man )  {
        m_origItem = item;
        setItem( item );
    }
    OKeyListViewItem::~OKeyListViewItem() {}
    OKeyConfigItem &OKeyListViewItem::item(){
        return m_item;
    }
    OKeyConfigItem  OKeyListViewItem::origItem() const{
        return m_origItem;
    }
    OKeyConfigManager* OKeyListViewItem::manager() {
        return m_manager;
    }
    void OKeyListViewItem::setItem( const OKeyConfigItem& item ) {
        m_item = item;
        setPixmap( 0, m_item.pixmap() );
        setText  ( 1, m_item.text() );
        m_item.keyPair().isEmpty() ? setText( 2, QObject::tr( "None" ) ) :
                                     setText( 2, keyToString( m_item.keyPair() ) );

        m_item.defaultKeyPair().isEmpty() ? setText( 3, QObject::tr( "None" ) ) :
                                            setText  ( 3, keyToString( m_item.defaultKeyPair() ) );
    }
    void OKeyListViewItem::updateText() {
        m_item.keyPair().isEmpty() ? setText( 2, QObject::tr( "None" ) ) :
                                     setText( 2, keyToString( m_item.keyPair() ) );
    }

    QString keyToString( const OKeyPair& pair ) {
        int mod = 0;
        if ( pair.modifier() & Qt::ShiftButton )
            mod |= Qt::SHIFT;
        if ( pair.modifier() & Qt::ControlButton )
            mod |= Qt::CTRL;
        if ( pair.modifier() & Qt::AltButton )
            mod |= Qt::ALT;

        return QAccel::keyToString( mod + pair.keycode() );
    }

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

    struct OKeyConfigWidgetPrivate{
        OKeyConfigWidgetPrivate(const QString& = QString::null,
                                OKeyConfigManager* = 0);
        bool operator==( const OKeyConfigWidgetPrivate& );
        QString name;
        OKeyConfigManager *manager;
    };

    OKeyConfigWidgetPrivate::OKeyConfigWidgetPrivate( const QString& _name,
                                                      OKeyConfigManager* man )
        : name( _name ), manager( man ){}

    bool OKeyConfigWidgetPrivate::operator==( const OKeyConfigWidgetPrivate& item) {
        if ( manager != item.manager) return false;
        if ( name    !=  item.name  ) return false;

        return true;
    }

}
}
}



////////////////////////
////////////////////////
//////// Widget Starts Here




/**
 *
 * This is a c'tor. You still need to pass the OKeyConfigManager
 * and then issue a load.
 * The default mode is Immediate
 *
 */
OKeyConfigWidget::OKeyConfigWidget( QWidget* parent, const char *name, WFlags fl )
    : QWidget( parent, name, fl ) {
    initUi();
}



/**
 * c'tor
 */
OKeyConfigWidget::~OKeyConfigWidget() {
}


/**
 * @internal
 */
void OKeyConfigWidget::initUi() {
    QBoxLayout *layout   = new QVBoxLayout( this );
    QGridLayout *gridLay = new QGridLayout( 2, 2 );
    layout->addLayout( gridLay, 10 );
    gridLay->setRowStretch( 1, 10 ); // let only the ListView strecth

/*
 * LISTVIEW with the Keys
 */
    m_view = new Opie::Ui::OListView( this );
    m_view->setFocus();
    m_view->setAllColumnsShowFocus( true );
    m_view->addColumn( tr("Pixmap") );
    m_view->addColumn( tr("Name","Name of the Action in the ListView Header" ) );
    m_view->addColumn( tr("Key" ) );
    m_view->addColumn( tr("Default Key" ) );
    m_view->setRootIsDecorated( true );
    connect(m_view, SIGNAL(currentChanged(QListViewItem*)),
            this, SLOT(slotListViewItem(QListViewItem*)) );

    gridLay->addMultiCellWidget( m_view, 1, 1, 0, 1 );

/*
 * GROUP with button info
 */

    QGroupBox *box = new QGroupBox( this );
    box ->setTitle(  tr("Shortcut for Selected Action") );
    box ->setFrameStyle( QFrame::Box | QFrame::Sunken );
    layout->addWidget( box, 1 );

    gridLay = new QGridLayout( box, 3, 4 );
    gridLay->addRowSpacing( 0, fontMetrics().lineSpacing() );
    gridLay->setMargin( 4 );

    QButtonGroup *gr = new QButtonGroup( box );
    gr->hide();
    gr->setExclusive( true );

    QRadioButton *rad = new QRadioButton( tr( "&None" ), box );
    connect( rad, SIGNAL(clicked()),
             this, SLOT(slotNoKey()) );
    gr->insert( rad, 10 );
    gridLay->addWidget( rad, 1, 0 );
    m_none = rad;

    rad = new QRadioButton( tr("&Default" ), box );
    connect( rad, SIGNAL(clicked()),
             this, SLOT(slotDefaultKey()) );
    gr->insert( rad, 11 );
    gridLay->addWidget( rad, 1, 1 );
    m_def = rad;

    rad = new QRadioButton( tr("C&ustom"), box );
    connect( rad, SIGNAL(clicked()),
             this, SLOT(slotCustomKey()) );
    gr->insert( rad, 12 );
    gridLay->addWidget( rad, 1, 2 );
    m_cus = rad;

    m_btn = new QPushButton( tr("Configure Key"), box );
    gridLay->addWidget( m_btn, 1, 4 );

    m_lbl= new QLabel( tr( "Default: " ), box );
    gridLay->addWidget( m_lbl, 2, 0 );

    connect(m_btn, SIGNAL(clicked()),
            this, SLOT(slotConfigure()));

    m_box = box;
}

/**
 * Set the ChangeMode.
 * You need to call this function prior to load
 * If you call this function past load the behaviour is undefined
 * But caling load again is safe
 */
void OKeyConfigWidget::setChangeMode( enum ChangeMode mode) {
    m_mode = mode;
}


/**
 * return the current mode
 */
OKeyConfigWidget::ChangeMode OKeyConfigWidget::changeMode()const {
    return m_mode;
}


/**
 * insert these items before calling load
 */
void OKeyConfigWidget::insert( const QString& str, OKeyConfigManager* man ) {
    Opie::Ui::Private::OKeyConfigWidgetPrivate root( str, man );
    m_list.append(root);
}


/**
 * loads the items and allows editing them
 */
void OKeyConfigWidget::load() {
    Opie::Ui::Private::OKeyConfigWidgetPrivateList::Iterator it;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        OListViewItem *item = new OListViewItem( m_view, (*it).name );
        OKeyConfigItem::List list = (*it).manager->keyConfigList();
        for (OKeyConfigItem::List::Iterator keyIt = list.begin(); keyIt != list.end();++keyIt )
            (void )new  Opie::Ui::Private::OKeyListViewItem(*keyIt, (*it).manager, item );

    }
}

/**
 * Saves if in Queue Mode. It'll update the supplied
 * OKeyConfigManager objects.
 * If in Queue mode it'll just return
 */
void OKeyConfigWidget::save() {
    /*
     * Iterate over all config items
     */
    QListViewItemIterator it( m_view );
    while ( it.current() ) {
        if (it.current()->parent() ) {
            Opie::Ui::Private::OKeyListViewItem *item = static_cast<Opie::Ui::Private::OKeyListViewItem*>( it.current() );
            OKeyConfigManager *man = item->manager();
            man->removeKeyConfig( item->origItem() );
            man->addKeyConfig( item->item() );
        }
        ++it;
    }


}


/**
 * @internal
 */
void OKeyConfigWidget::slotListViewItem( QListViewItem* _item) {
    if ( !_item || !_item->parent() ) {
        m_box->setEnabled( false );
        m_none->setChecked( true );
        m_btn ->setEnabled( false );
        m_def ->setChecked( false );
        m_cus ->setChecked( false );
    }else{
        m_box->setEnabled( true );
        Opie::Ui::Private::OKeyListViewItem *item = static_cast<Opie::Ui::Private::OKeyListViewItem*>( _item );
        OKeyConfigItem keyItem= item->item();
        m_lbl->setText(  tr("Default: " )+ item->text( 3 ) );
        if ( keyItem.keyPair().isEmpty() ) {
            m_none->setChecked( true );
            m_btn ->setEnabled( false );
            m_def ->setChecked( false );
            m_cus ->setChecked( false );
        }else {
            m_none->setChecked( false );
            m_cus ->setChecked( true );
            m_btn ->setEnabled( true );
            m_def ->setChecked( false );
        }
    }
}

void OKeyConfigWidget::slotNoKey() {
    m_none->setChecked( true );
    m_cus ->setChecked( false );
    m_btn ->setEnabled( false );
    m_def ->setChecked( false );

    if ( !m_view->currentItem() || !m_view->currentItem()->parent() )
        return;



    /*
     * If immediate we need to remove and readd the key
     */
    Opie::Ui::Private::OKeyListViewItem *item =  static_cast<Opie::Ui::Private::OKeyListViewItem*>(m_view->currentItem());
    updateItem( item, OKeyPair::emptyKey() );
}

void OKeyConfigWidget::slotDefaultKey() {
    m_none->setChecked( false );
    m_cus ->setChecked( false );
    m_btn ->setEnabled( false );
    m_def ->setChecked( true );

    if ( !m_view->currentItem() || !m_view->currentItem()->parent() )
        return;

    Opie::Ui::Private::OKeyListViewItem *item =  static_cast<Opie::Ui::Private::OKeyListViewItem*>(m_view->currentItem());
    updateItem( item, item->item().defaultKeyPair() );
}

void OKeyConfigWidget::slotCustomKey() {
    m_cus ->setChecked( true );
    m_btn ->setEnabled( true );
    m_def ->setChecked( false );
    m_none->setChecked( false );

    if ( !m_view->currentItem() || !m_view->currentItem()->parent() )
        return;


}

void OKeyConfigWidget::slotConfigure() {
    if ( !m_view->currentItem() || !m_view->currentItem()->parent() )
        return;

   /* FIXME make use of OModalHelper */
    OKeyChooserConfigDialog dlg( this, "Dialog Name", true );
    dlg.setCaption(tr("Configure Key"));
    connect(&dlg, SIGNAL(keyCaptured()), &dlg, SLOT(accept()) );

    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        Opie::Ui::Private::OKeyListViewItem *item = static_cast<Opie::Ui::Private::OKeyListViewItem*>(m_view->currentItem());
        updateItem( item, dlg.keyPair() );
    }


}

bool OKeyConfigWidget::sanityCheck(  Opie::Ui::Private::OKeyListViewItem* item,
                                     const OKeyPair& newItem ) {
    OKeyPair::List bList = item->manager()->blackList();
    for ( OKeyPair::List::Iterator it = bList.begin(); it != bList.end(); ++it ) {
        /* black list matched */
        if ( *it == newItem ) {
            QMessageBox::warning( 0, tr("Key is on BlackList" ),
                                  tr("<qt>The Key you choose is on the black list "
                                     "and may not be used with this manager. Please "
                                     "use a different key.</qt>" ) );
            return false;
        }
    }
    /* no we need to check the other items which is dog slow */
    QListViewItemIterator it( item->parent() );
    while ( it.current() ) {
        /* if not our parent and not us */
        if (it.current()->parent() &&  it.current() != item) {
            /* damn already given away*/
            if ( newItem == static_cast<Opie::Ui::Private::OKeyListViewItem*>(it.current() )->item().keyPair() ) {
                QMessageBox::warning( 0, tr("Key is already assigned" ),
                                      tr("<qt>The Key you choose is already taken by "
                                         "a different Item of your config. Please try"
                                         "using a different key.</qt>" ) );
                return false;
            }
        }
        ++it;
    }

    return true;
}

void OKeyConfigWidget::updateItem( Opie::Ui::Private::OKeyListViewItem *item,
                                   const OKeyPair& newItem) {
    /* sanity check
     * check against the blacklist of the manager
     * check if another item uses this key which is o(n) at least
     */
    if ( !newItem.isEmpty() && !sanityCheck(item, newItem ))
        return;



    /*
    * If immediate we need to remove and readd the key
    */
    if ( m_mode == Imediate )
        item->manager()->removeKeyConfig( item->item() );

    item->item().setKeyPair( newItem );
    item->updateText();

    if ( m_mode == Imediate )
        item->manager()->addKeyConfig( item->item() );
}



/////
OKeyChooserConfigDialog::OKeyChooserConfigDialog( QWidget* par, const char* nam,
                                                  bool mod, WFlags fl )
    : QDialog( par, nam, mod, fl ), m_virtKey( false ), m_keyPair( OKeyPair::emptyKey() ) ,
      m_key( 0 ), m_mod( 0 ) {
    setFocusPolicy( StrongFocus );

    QHBoxLayout *lay = new QHBoxLayout( this );

    QLabel *lbl = new QLabel( tr("Configure Key" ), this );
    lay->addWidget( lbl );
    lbl->setFocusPolicy( NoFocus );

    m_lbl = new QLabel( this );
    lay->addWidget( m_lbl );
    m_lbl->setFocusPolicy( NoFocus );

    m_timer = new QTimer( this );
    connect(m_timer, SIGNAL(timeout()),
            this, SLOT(slotTimeUp()) );
}

OKeyChooserConfigDialog::~OKeyChooserConfigDialog() {
}

Opie::Ui::OKeyPair OKeyChooserConfigDialog::keyPair()const{
    return m_keyPair;
}

void OKeyChooserConfigDialog::keyPressEvent( QKeyEvent* ev ) {
    QDialog::keyPressEvent( ev );

    if ( ev->isAutoRepeat() )
        return;

    int mod, key;
    Opie::Ui::Private::fixupKeys( key,mod, ev );

    /* either we used software keyboard
     * or we've true support
     */
    if ( !m_virtKey && !ev->key()) {
        m_virtKey = true;
        m_keyPair = OKeyPair( key, mod );
    }else{
        mod = 0;
        switch( key ) {
        case Qt::Key_Control:
            mod = Qt::ControlButton;
            break;
        case Qt::Key_Shift:
            mod = Qt::ShiftButton;
            break;
        case Qt::Key_Alt:
            mod = Qt::AltButton;
            break;
        default:
            break;
        }
        if (mod )
            m_mod |= mod;
        else
            m_key = key;

        if ( ( !mod || m_key ) && !m_timer->isActive() )
            m_timer->start( 150, true );

        m_keyPair = OKeyPair( m_key, m_mod );
    }

    m_lbl->setText( Opie::Ui::Private::keyToString( m_keyPair ) );

}

void OKeyChooserConfigDialog::keyReleaseEvent( QKeyEvent* ev ) {
    m_timer->stop();
    QDialog::keyPressEvent( ev );

    if ( ev->isAutoRepeat() )
        return;


    if ( m_virtKey && !ev->key()) {
        m_virtKey = false;
        slotTimeUp();
    }else {
        int mod = 0;
        int key = ev->key();
        switch( key ) {
        case Qt::Key_Control:
            mod = Qt::ControlButton;
            break;
        case Qt::Key_Shift:
            mod = Qt::ShiftButton;
            break;
        case Qt::Key_Alt:
            mod = Qt::AltButton;
            break;
        default:
            break;
        }
        if (mod )
            m_mod &= ~mod;
        else
            m_key = key;
       m_keyPair = OKeyPair( m_key, m_mod );
       m_lbl->setText( Opie::Ui::Private::keyToString( m_keyPair ) );
    }
}


void OKeyChooserConfigDialog::slotTimeUp() {
    m_mod = m_key = 0;
    QTimer::singleShot(0, this, SIGNAL(keyCaptured()) );
}
