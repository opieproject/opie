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

using Opie::Core::OKeyConfigItem;
using Opie::Core::OKeyPair;
using Opie::Core::OKeyConfigManager;

namespace Opie {
namespace Ui {
namespace Internal {

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
    Opie::Ui::Internal::OKeyConfigWidgetPrivate root( str, man );
    m_list.append(root);
}


/**
 * loads the items and allows editing them
 */
void OKeyConfigWidget::load() {
    Opie::Ui::Internal::OKeyConfigWidgetPrivateList::Iterator it;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        OListViewItem *item = new OListViewItem( m_view, (*it).name );
        OKeyConfigItem::List list = (*it).manager->keyConfigList();
        for (OKeyConfigItem::List::Iterator keyIt = list.begin(); keyIt != list.end();++keyIt )
            (void )new  Opie::Ui::Internal::OKeyListViewItem(*keyIt, (*it).manager, item );

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
            Opie::Ui::Internal::OKeyListViewItem *item = static_cast<Opie::Ui::Internal::OKeyListViewItem*>( it.current() );
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
        Opie::Ui::Internal::OKeyListViewItem *item = static_cast<Opie::Ui::Internal::OKeyListViewItem*>( _item );
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
    Opie::Ui::Internal::OKeyListViewItem *item =  static_cast<Opie::Ui::Internal::OKeyListViewItem*>(m_view->currentItem());
    updateItem( item, OKeyPair::emptyKey() );
}

void OKeyConfigWidget::slotDefaultKey() {
    m_none->setChecked( false );
    m_cus ->setChecked( false );
    m_btn ->setEnabled( false );
    m_def ->setChecked( true );

    if ( !m_view->currentItem() || !m_view->currentItem()->parent() )
        return;

    Opie::Ui::Internal::OKeyListViewItem *item =  static_cast<Opie::Ui::Internal::OKeyListViewItem*>(m_view->currentItem());
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
        Opie::Ui::Internal::OKeyListViewItem *item = static_cast<Opie::Ui::Internal::OKeyListViewItem*>(m_view->currentItem());
        updateItem( item, dlg.keyPair() );
    }


}

bool OKeyConfigWidget::sanityCheck(  Opie::Ui::Internal::OKeyListViewItem* item,
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
            if ( newItem == static_cast<Opie::Ui::Internal::OKeyListViewItem*>(it.current() )->item().keyPair() ) {
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

void OKeyConfigWidget::updateItem( Opie::Ui::Internal::OKeyListViewItem *item,
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

Opie::Core::OKeyPair OKeyChooserConfigDialog::keyPair()const{
    return m_keyPair;
}

void OKeyChooserConfigDialog::keyPressEvent( QKeyEvent* ev ) {
    QDialog::keyPressEvent( ev );

    if ( ev->isAutoRepeat() )
        return;

    int mod, key;
    Opie::Core::Internal::fixupKeys( key,mod, ev );

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
        if (mod ) {
            m_mod |= mod;
            key = 0;
        }else
            m_key = key;

        if ( ( !mod || m_key || key ) && !m_timer->isActive() )
            m_timer->start( 150, true );

        m_keyPair = OKeyPair( m_key, m_mod );
    }

    m_lbl->setText( Opie::Ui::Internal::keyToString( m_keyPair ) );

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
       m_lbl->setText( Opie::Ui::Internal::keyToString( m_keyPair ) );
    }
}


void OKeyChooserConfigDialog::slotTimeUp() {
    m_mod = m_key = 0;
    QTimer::singleShot(0, this, SIGNAL(keyCaptured()) );
}


}
}
