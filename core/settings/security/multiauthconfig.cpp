#include <opie2/odebug.h>
#include <opie2/multiauthpassword.h>

#include <qgroupbox.h>
#include <qvgroupbox.h>
#include <qpe/resource.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qheader.h>
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qtoolbutton.h>
#include <qstringlist.h>
#include <qdir.h>
#include <qpe/qlibrary.h>
#include <qpe/qpeapplication.h>

#include "multiauthconfig.h"


using Opie::Security::MultiauthPluginInterface;
using Opie::Security::MultiauthPluginObject;
using Opie::Security::MultiauthConfigWidget;
/// keeps information about MultiauthPluginObject plugins
struct MultiauthPlugin {
    MultiauthPlugin() : library( 0 ), iface( 0 ), pluginObject( 0 ) {}
    /// plugin file
    QLibrary *library;
    /// the plugin object interface
    QInterfacePtr<MultiauthPluginInterface> iface;
    /// the plugin object itself
    MultiauthPluginObject *pluginObject;
    /// name of the plugin file
    QString name;
    /// should the plugin be launched during authentication or not
    bool active;
    /// order of the plugin, in the pluginListWidget and during authentication
    int pos;
};

/// list of available MultiauthPlugin objects
static QValueList<MultiauthPlugin> pluginList;


/// extension of QToolButton that adds signals, icons and stuff (taken from todayconfig.cpp)
class ToolButton : public QToolButton {

    public:
        ToolButton( QWidget *parent, const char *name, const QString& icon, QObject *handler, const QString& slot, bool t = FALSE )
            : QToolButton( parent, name ) {
                setPixmap( Resource::loadPixmap( icon ) );
                setAutoRaise( TRUE );
                setFocusPolicy( QWidget::NoFocus );
                setToggleButton( t );
                connect( this, t ? SIGNAL( toggled(bool) ) : SIGNAL( clicked() ), handler, slot );
            }
};

    MultiauthGeneralConfig::MultiauthGeneralConfig(MultiauthConfig * parentConfig, QWidget * parent, const char * name = "general Opie-multiauthentication config widget")
: QWidget(parent, name), m_onStart(0), m_onResume(0), m_noProtectConfig(0), m_explanScreens(0), m_nbSuccessMin(0), m_tryButton(0)
{
    // keep track of the MultiauthConfig parent in one of our attributes
    m_parentConfig = parentConfig;
    QVBoxLayout *vb = new QVBoxLayout(this);
    vb->setSpacing(11);
    vb->setMargin(11);
    vb->setAlignment( Qt::AlignTop );

    QGroupBox *lockBox = new QGroupBox(0, Qt::Vertical, tr("When to lock Opie"), this, "lock box");
    vb->addWidget(lockBox);
    QGridLayout *boxLayout = new QGridLayout( lockBox->layout() );
    m_onStart = new QCheckBox( tr( "on Opie start" ), lockBox, "lock on opie start");
    m_onResume = new QCheckBox( tr( "on Opie resume" ), lockBox, "lock on opie resume");
    boxLayout->addWidget(m_onStart, 0, 0);
    boxLayout->addWidget(m_onResume, 0, 1);

    QGroupBox *nbBox = new QGroupBox(0, Qt::Vertical, tr("Multiple plugins authentication"), this, "nb box");
    vb->addWidget(nbBox);
    QGridLayout *nbBoxLayout = new QGridLayout( nbBox->layout() );
    m_nbSuccessMin = new QSpinBox(nbBox);
    QLabel *lNbSuccessMin = new QLabel( tr( "Required successes" ), nbBox);
    nbBoxLayout->addWidget(m_nbSuccessMin, 0, 0);
    nbBoxLayout->addWidget(lNbSuccessMin, 0, 1);
    m_nbSuccessMin->setMinValue(1); // the max value is defined in MultiauthConfig constructor

    QGroupBox *devBox = new QGroupBox(0, Qt::Vertical, tr("Options"), this, "dev box");
    vb->addWidget(devBox);
    QGridLayout *devBoxLayout = new QGridLayout( devBox->layout() );
    m_noProtectConfig = new QCheckBox( tr("Don't protect this config screen"), devBox, "don't protect config");
    m_explanScreens = new QCheckBox( tr("Show explanatory screens"), devBox, "Show explan. screens");
    devBoxLayout->addWidget(m_noProtectConfig, 0, 0);
    devBoxLayout->addWidget(m_explanScreens, 1, 0);

    QVGroupBox *tryBox = new QVGroupBox(tr("Testing"), this, "try box");
    vb->addWidget(tryBox);
    m_tryButton = new QPushButton( tr("Test the authentication now"), tryBox, "try button");
    connect( m_tryButton, SIGNAL(clicked()), this, SLOT(tryAuth()) );

}

/// nothing to do
MultiauthGeneralConfig::~MultiauthGeneralConfig()
{}

/// launches the authentication process, as configured, with the option to bypass it
void MultiauthGeneralConfig::tryAuth()
{
    QMessageBox confirmSave(
                         tr("Attention"),
                         "<p>" + tr("You must save your current settings before trying to authenticate. Press OK to accept and launch a simulated authentication process.") + "</p><p><em>" +
                         tr("If you don't like the result of this test, don't forget to change your settings before you exit the configuration application!") + "</em></p>",
                         QMessageBox::Warning,
                         QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::NoButton,
                         0, QString::null, TRUE, WStyle_StaysOnTop);
    confirmSave.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    confirmSave.setButtonText(QMessageBox::Yes, tr("OK"));

    if ( confirmSave.exec() == QMessageBox::Yes)
    {
        owarn << "writing config as user accepted" << oendl;
        m_parentConfig->writeConfigs();
        owarn << "testing authentication" << oendl;
        // launch the authentication in testing mode
        Opie::Security::MultiauthPassword::authenticate(Opie::Security::TestNow);
    }
}


/// Builds and displays the Opie multi-authentication configuration dialog
static void test_and_start() {
    Config pcfg("Security");
    pcfg.setGroup( "Misc" );
    bool protectConfigDialog = ! pcfg.readBoolEntry("noProtectConfig", true);

    if (protectConfigDialog && Opie::Security::Internal::runPlugins() != 0) {
        owarn << "authentication failed, not showing opie-security" << oendl;
        exit( -1 );
    }
}



    MultiauthConfig::MultiauthConfig(QWidget* par, const char* w = "MultiauthConfig dialog",  WFlags f = 0)
: QDialog(par, w, TRUE, f),
    m_mainTW(0), m_pluginListView(0), m_pluginListWidget(0),
    m_generalConfig(0), m_loginWidget(0), m_syncWidget(0),
    m_nbSuccessReq(0), m_plugins_changed(false)
{
    /* Initializes the global configuration window
    */
    test_and_start();

    /* Checks (and memorizes) if any authentication plugins are
     * installed on the system
     */
    QString path = QPEApplication::qpeDir() + "plugins/security";
    QDir dir( path, "lib*.so" );
    QStringList list = dir.entryList();

    m_pluginsInstalled = ! list.isEmpty();
    if (m_pluginsInstalled == false)
        owarn << "no authentication plugins installed! Talking about it in the last tab..." << oendl;

    setCaption( tr( "Security configuration" ) );
    QVBoxLayout *layout = new QVBoxLayout( this );
    m_mainTW = new Opie::Ui::OTabWidget( this, "main tab widget" );
    layout->addWidget(m_mainTW);

    if (m_pluginsInstalled)
    {
        m_pluginListWidget = new QWidget(m_mainTW, "plugin list widget");
        QVBoxLayout * pluginListLayout = new QVBoxLayout(m_pluginListWidget);
        pluginListLayout->setSpacing(6);
        pluginListLayout->setMargin(11);
        QLabel * pluginListTitle = new QLabel( tr( "Load which plugins in what order:" ), m_pluginListWidget );
        pluginListLayout->addWidget(pluginListTitle);
        QHBox * pluginListHB = new QHBox(m_pluginListWidget);
        pluginListLayout->addWidget(pluginListHB);

        m_pluginListView = new QListView(pluginListHB);
        m_pluginListView->addColumn("PluginList");
        m_pluginListView->header()->hide();
        m_pluginListView->setSorting(-1);
        QWhatsThis::add(m_pluginListView, tr( "Check a checkbox to activate/deactivate a plugin or use the arrow buttons on the right to change the order they will appear in" ));

        QVBox * pluginListVB = new QVBox(pluginListHB);
        new ToolButton( pluginListVB, tr( "Move Up" ), "up",  this , SLOT( moveSelectedUp() ) );
        new ToolButton( pluginListVB, tr( "Move Down" ), "down", this , SLOT( moveSelectedDown() ) );
        m_mainTW->addTab( m_pluginListWidget, "pass", tr( "plugins" ) );

        connect ( m_pluginListView , SIGNAL( clicked ( QListViewItem * ) ), this, SLOT( pluginsChanged ( ) ) );

        // general Opie multi-authentication configuration tab
        m_generalConfig = new MultiauthGeneralConfig(this, m_mainTW);
        m_mainTW->addTab(m_generalConfig, "SettingsIcon", tr( "Authentication") );

    }
    // login settings page
    m_loginWidget = new LoginBase(m_mainTW, "login config widget");
    m_mainTW->addTab(m_loginWidget, "security/users", tr( "Login") );

    // sync settings page
    m_syncWidget = new SyncBase( m_mainTW, "sync config widget" );
    m_mainTW->addTab(m_syncWidget, "security/sync", tr( "Sync") );

    // read the "Security" Config file and update our UI
    readConfig();


    if (m_pluginsInstalled)
    {
        /* loads plugins configuration widgets in mainTW tabs and in pluginListView
        */

        loadPlugins();

        for ( int i = pluginList.count() - 1; i >= 0; i-- ) {
            MultiauthPlugin plugin = pluginList[i];

            // load the config widgets in the tabs
            // (configWidget will return 0l if there is no configuration GUI)
            MultiauthConfigWidget* widget = plugin.pluginObject->configWidget(m_mainTW);
            if ( widget != 0l ) {
                odebug << "plugin " << plugin.name << " has a configuration widget" << oendl;
                configWidgetList.append(widget);
                m_mainTW->addTab( widget, plugin.pluginObject->pixmapNameConfig(),
                                  plugin.pluginObject->pluginName() );
            }
            // set the order/activate tab
            QPixmap icon = Resource::loadPixmap( plugin.pluginObject->pixmapNameWidget() );
            QCheckListItem * item = new QCheckListItem(m_pluginListView, plugin.pluginObject->pluginName(), QCheckListItem::CheckBox );
            if ( !icon.isNull() ) {
                item->setPixmap( 0, icon );
            }
            if ( m_excludePlugins.find( plugin.name ) == m_excludePlugins.end() ) {
                item->setOn( TRUE );
            }
            m_plugins[plugin.name] = item;
        }

        // set the first tab as default.
        m_mainTW->setCurrentTab(m_pluginListWidget);

        // put the number of plugins as the max number of req. auth.
        m_generalConfig->m_nbSuccessMin->setMaxValue( pluginList.count() );
    }
    else
    {
        /* we don't have any installed plugin there. Let's tell
         * that to the user in a third tab, using the m_pluginListWidget widget
         */
        m_pluginListWidget = new QWidget(m_mainTW, "plugin list widget (no plugins warning)");
        QVBoxLayout * pluginListLayout = new QVBoxLayout(m_pluginListWidget);
        pluginListLayout->setSpacing(11);
        pluginListLayout->setMargin(11);
        pluginListLayout->setAlignment( Qt::AlignTop );
        QVGroupBox *warningBox = new QVGroupBox(tr("Important notice"), m_pluginListWidget, "noPlugins warning box");
        pluginListLayout->addWidget(warningBox);
        QLabel * warningText = new QLabel( "<p>" + tr("To be able to protect your PDA with one or more authentication plugins (for example, a simple PIN authentication), you must install at least one <em>opie-securityplugin-*</em> package! Once you have done that, you will be able to configure your PDA protection here.") + "</p>", warningBox );

        m_mainTW->addTab(m_pluginListWidget, "security/Security", tr( "Locking") );

        // set the first tab as default.
        m_mainTW->setCurrentTab(m_loginWidget);
    }

    showMaximized();
}

/// nothing to do
MultiauthConfig::~MultiauthConfig()
{
}

/// saves the general and plugin(s) configurations
void MultiauthConfig::writeConfigs() {
    MultiauthConfigWidget* confWidget = 0;
    for ( confWidget = configWidgetList.first(); confWidget != 0;
          confWidget = configWidgetList.next() )
        confWidget->writeConfig();

    writeConfig();
}

/// on QDialog::accept, we save all the configurations and exit the QDialog normally
void MultiauthConfig::accept() {
    writeConfigs();
    QDialog::accept();
}

void MultiauthConfig::done( int r ) {
    QDialog::done( r );
    close();
}

/// moves up the selected plugin
void MultiauthConfig::moveSelectedUp()
{
    QListViewItem *item = m_pluginListView->selectedItem();
    if ( item && item->itemAbove() ) {
        item->itemAbove()->moveItem( item );
    }
}

/// moves down the selected plugin
void MultiauthConfig::moveSelectedDown()
{
    QListViewItem *item = m_pluginListView->selectedItem();
    if ( item && item->itemBelow() ) {
        item->moveItem( item->itemBelow() );
    }
}

/// reads the <code>Security.conf</code> Config file, and updates parts of the user interface
void MultiauthConfig::readConfig()
{
    // pointer, so we release this Config when we want
    Config* pcfg = new Config("Security");

    if (m_pluginsInstalled)
    {
        pcfg->setGroup( "Misc" );
        m_generalConfig->m_onStart->setChecked( pcfg->readBoolEntry( "onStart", false ) );
        m_generalConfig->m_onResume->setChecked( pcfg->readBoolEntry( "onResume", false ) );
        m_generalConfig->m_nbSuccessMin->setValue( pcfg->readNumEntry( "nbSuccessMin", 1 ) );
        m_generalConfig->m_noProtectConfig->setChecked( pcfg->readBoolEntry( "noProtectConfig", true) );
        m_generalConfig->m_explanScreens->setChecked( pcfg->readBoolEntry( "explanScreens", true ) );

        pcfg->setGroup( "Plugins" );
        m_excludePlugins = pcfg->readListEntry( "ExcludePlugins", ',' );
        m_allPlugins = pcfg->readListEntry( "AllPlugins", ',' );
    }

    /* Login and Sync stuff */
    pcfg->setGroup("Sync");
    int auth_peer = pcfg->readNumEntry("auth_peer",0xc0a88100);//new default 192.168.129.0/24
    int auth_peer_bits = pcfg->readNumEntry("auth_peer_bits",24);

    pcfg->setGroup("SyncMode");
    int mode = pcfg->readNumEntry("Mode",2); // Default to Sharp
    switch( mode ) {
        case 0x01:
            m_syncWidget->syncModeCombo->setCurrentItem( 0 );
            break;
        case 0x02:
        default:
            m_syncWidget->syncModeCombo->setCurrentItem( 1 );
            break;
        case 0x04:
            m_syncWidget->syncModeCombo->setCurrentItem( 2 );
            break;
    }
    /*
       cfg.setGroup("Remote");
       if ( telnetAvailable() )
       telnet->setChecked(cfg.readEntry("allow_telnet"));
       else
       telnet->hide();

       if ( sshAvailable() )
       ssh->setChecked(cfg.readEntry("allow_ssh"));
       else
       ssh->hide();
       */

    // release the Config handler
    delete pcfg;
    // indeed, selectNet will open the config file...
    selectNet(auth_peer,auth_peer_bits,TRUE);

    connect( m_syncWidget->syncnet, SIGNAL(textChanged(const QString&)),
             this, SLOT(setSyncNet(const QString&)));



    QString configFile = QPEApplication::qpeDir() + "etc/opie-login.conf";
    Config loginCfg(configFile,Config::File);

    loginCfg.setGroup("General");
    autoLoginName=loginCfg.readEntry("AutoLogin","");

    if (autoLoginName.stripWhiteSpace().isEmpty()) {
        autoLogin=false;
    } else {
        autoLogin=true;
    }


    connect(m_loginWidget->autologinToggle, SIGNAL(toggled(bool)), this, SLOT(toggleAutoLogin(bool)));
    connect(m_loginWidget->userlist, SIGNAL(activated(int)), this, SLOT(changeLoginName(int)));
    connect(m_syncWidget->restoredefaults,SIGNAL(clicked()), this, SLOT(restoreDefaults()));
    connect(m_syncWidget->deleteentry,SIGNAL(clicked()), this, SLOT(deleteListEntry()));

    loadUsers();
    updateGUI();

}

void MultiauthConfig::writeConfig()
{
    Config pcfg("Security");

    if (m_pluginsInstalled)
    {
        pcfg.setGroup( "Plugins" );
        QStringList exclude;
        QStringList include;
        QStringList allPlugins;

        QListViewItemIterator list_it( m_pluginListView );

        // this makes sure the names get saved in the order selected
        for ( ; list_it.current(); ++list_it ) {
            QMap <QString, QCheckListItem *>::Iterator it;
            for ( it = m_plugins.begin(); it != m_plugins. end (); ++it ) {
                if ( list_it.current() == (*it) && !(*it)-> isOn () ) {
                    exclude << it.key();
                } else if ( list_it.current() == (*it) && (*it)-> isOn () ){
                    include << it.key();
                }
                if ( list_it.current() == (*it) ) {
                    allPlugins << it.key();
                }
            }
        }
        pcfg.writeEntry( "ExcludePlugins", exclude, ',' );
        pcfg.writeEntry( "IncludePlugins", include, ',' );
        pcfg.writeEntry( "AllPlugins",  allPlugins, ',' );

        pcfg.setGroup( "Misc" );
        pcfg.writeEntry( "onStart",  m_generalConfig->m_onStart->isChecked() );
        pcfg.writeEntry( "onResume",  m_generalConfig->m_onResume->isChecked() );
        pcfg.writeEntry( "nbSuccessMin",  m_generalConfig->m_nbSuccessMin->text() );
        pcfg.writeEntry( "noProtectConfig",  m_generalConfig->m_noProtectConfig->isChecked() );
        pcfg.writeEntry( "explanScreens",  m_generalConfig->m_explanScreens->isChecked() );
    }

    /* Login and Sync stuff */

    pcfg.setGroup("Sync");
    int auth_peer=0;
    int auth_peer_bits;
    QString sn = m_syncWidget->syncnet->currentText();
    parseNet(sn,auth_peer,auth_peer_bits);

    //this is the *selected* (active) net range
    pcfg.writeEntry("auth_peer",auth_peer);
    pcfg.writeEntry("auth_peer_bits",auth_peer_bits);

    //write back all other net ranges in *cleartext*
    for (int i=0; i<10; i++) {
        QString target;
        target.sprintf("net%d", i);
        if ( i < m_syncWidget->syncnet->count() )
            pcfg.writeEntry(target, m_syncWidget->syncnet->text(i));
        else // no more entry in the syncnet list -> we clear the line
            pcfg.writeEntry(target, "");
    }

#ifdef ODP
#error "Use 0,1,2 and use Launcher"
#endif
    /* keep the old code so we don't use currentItem directly */
    int value = 0x02;
    switch( m_syncWidget->syncModeCombo->currentItem() ) {
        case 0:
            value = 0x01;
            break;
        case 1:
            value = 0x02;
            break;
        case 2:
            value = 0x04;
            break;
    }
    pcfg.setGroup("SyncMode");
    pcfg.writeEntry( "Mode", value );

    /*
       pcfg.setGroup("Remote");
       if ( telnetAvailable() )
       pcfg.writeEntry("allow_telnet",telnet->isChecked());
       if ( sshAvailable() )
       pcfg.writeEntry("allow_ssh",ssh->isChecked());
    // ### write ssh/telnet sys config files
    */

    //release the Config handler
    pcfg.write();

    QString configFile = QPEApplication::qpeDir() + "etc/opie-login.conf";
    Config loginCfg(configFile,Config::File);
    loginCfg.setGroup("General");

    if (autoLogin) {
        loginCfg.writeEntry("AutoLogin",autoLoginName);
    } else {
        loginCfg.removeEntry("AutoLogin");
    }

}

/// slot used to record the fact plugins order has been modified
void MultiauthConfig::pluginsChanged() {
    m_plugins_changed = true;
}

/// loads each multiauth plugin
void MultiauthConfig::loadPlugins() {

    QString path = QPEApplication::qpeDir() + "plugins/security";
    QDir dir( path, "lib*.so" );

    QStringList list = dir.entryList();
    QStringList::Iterator it;

    // temporary list used to sort plugins
    QMap<QString, MultiauthPlugin> sortList;

    for ( it = list.begin(); it != list.end(); ++it ) {
        QInterfacePtr<MultiauthPluginInterface> iface;
        QLibrary *lib = new QLibrary( path + "/" + *it );
        QString libPath(path + "/" + *it);

        if ( lib->queryInterface( IID_MultiauthPluginInterface, (QUnknownInterface**)&iface ) == QS_OK ) {
            MultiauthPlugin plugin;
            plugin.library = lib;
            plugin.iface = iface;
            plugin.name = QString(*it);

            // find out if plugins should be launched
            if ( m_excludePlugins.grep( *it ).isEmpty() ) {
                plugin.active = true;
            } else {
                plugin.active = false;
            }

            plugin.pluginObject = plugin.iface->plugin();

            // "prebuffer" it in one more list, to get the sorting done
            sortList.insert( plugin.name, plugin );

            // on first start the list is off course empty
            if ( m_allPlugins.isEmpty() ) {
                pluginList.append( plugin );
            }
            // if plugin is not yet in the list, add it to the layout too
            else if ( !m_allPlugins.contains( plugin.name ) ) {
                pluginList.append( plugin );
            }

        } else {
            delete lib;
        }

    } // end for

    // put m_allPlugins tempPlugin objects into pluginList
    if ( !m_allPlugins.isEmpty() ) {
        MultiauthPlugin tempPlugin;
        QStringList::Iterator stringit;
        for( stringit = m_allPlugins.begin(); stringit !=  m_allPlugins.end(); ++stringit ) {
            tempPlugin = ( sortList.find( *stringit ) ).data();
            if ( !( (tempPlugin.name).isEmpty() ) ) {
                pluginList.append( tempPlugin );
            }
        }
    }

}

void MultiauthConfig::deleteListEntry()
{
    m_syncWidget->syncnet->removeItem(m_syncWidget->syncnet->currentItem());
}

void MultiauthConfig::restoreDefaults()
{
    QMessageBox unrecbox(
                         tr("Attention"),
                         "<p>" + tr("All user-defined net ranges will be lost.") + "</p>",
                         QMessageBox::Warning,
                         QMessageBox::Cancel, QMessageBox::Yes, QMessageBox::NoButton,
                         0, QString::null, TRUE, WStyle_StaysOnTop);
    unrecbox.setButtonText(QMessageBox::Cancel, tr("Cancel"));
    unrecbox.setButtonText(QMessageBox::Yes, tr("OK"));

    if ( unrecbox.exec() == QMessageBox::Yes)
    {
        m_syncWidget->syncnet->clear();
        insertDefaultRanges();
    }
    m_syncWidget->syncModeCombo->setCurrentItem( 2 );
}

void MultiauthConfig::insertDefaultRanges()
{
    m_syncWidget->syncnet->insertItem( "192.168.129.0/24" );
    m_syncWidget->syncnet->insertItem( "192.168.1.0/24" );
    m_syncWidget->syncnet->insertItem( "192.168.0.0/16" );
    m_syncWidget->syncnet->insertItem( "172.16.0.0/12" );
    m_syncWidget->syncnet->insertItem( "10.0.0.0/8" );
    m_syncWidget->syncnet->insertItem( "1.0.0.0/8" );
    m_syncWidget->syncnet->insertItem( tr( "Any" ) );
    m_syncWidget->syncnet->insertItem( tr( "None" ) );
}

void MultiauthConfig::updateGUI()
{
    m_loginWidget->autologinToggle->setChecked(autoLogin);
    m_loginWidget->userlist->setEnabled(autoLogin);
}

void MultiauthConfig::selectNet(int auth_peer,int auth_peer_bits, bool update)
{
    QString sn;
    if ( auth_peer_bits == 0 && auth_peer == 0 ) {
        sn = tr("Any");
    } else if ( auth_peer_bits == 32 && auth_peer == 0 ) {
        sn = tr("None");
    } else {
        sn =
            QString::number((auth_peer>>24)&0xff) + "."
            + QString::number((auth_peer>>16)&0xff) + "."
            + QString::number((auth_peer>>8)&0xff) + "."
            + QString::number((auth_peer>>0)&0xff) + "/"
            + QString::number(auth_peer_bits);
    }

    //insert user-defined list of netranges upon start
    if (update) {
        //User selected/active netrange first
        m_syncWidget->syncnet->insertItem( tr(sn) );
        Config cfg("Security");
        cfg.setGroup("Sync");

        //set up defaults if needed, if someone manually deletes net0 he'll get a suprise hehe
        QString test = cfg.readEntry("net0","");
        if (test.isEmpty()) {
            insertDefaultRanges();
        } else {
            // 10 ought to be enough for everybody... :)
            // If you need more, don't forget to edit applySecurity() as well
            bool already_there=FALSE;
            for (int i=0; i<10; i++) {
                QString target, netrange;
                target.sprintf("net%d", i);
                netrange = cfg.readEntry(target,"");
                if (! netrange.isEmpty()){
                    //make sure we have no "twin" entries
                    for (int i=0; i<m_syncWidget->syncnet->count(); i++) {
                        if ( m_syncWidget->syncnet->text(i) == netrange ) {
                            already_there=TRUE;
                        }
                    }
                    if (! already_there) {
                        m_syncWidget->syncnet->insertItem( netrange );
                    } else {
                        already_there=FALSE;
                    }
                }
            }
        }
    }

    for (int i=0; i<m_syncWidget->syncnet->count(); i++) {
        if ( m_syncWidget->syncnet->text(i).left(sn.length()) == sn ) {
            m_syncWidget->syncnet->setCurrentItem(i);
            return;
        }
    }
    odebug << "No match for \"" << sn << "\"" << oendl;
}

void MultiauthConfig::parseNet(const QString& sn,int& auth_peer,int& auth_peer_bits)
{
    auth_peer=0;
    if ( sn == tr("Any") ) {
        auth_peer = 0;
        auth_peer_bits = 0;
    } else if ( sn == tr("None") ) {
        auth_peer = 0;
        auth_peer_bits = 32;
    } else {
        int x=0;
        for (int i=0; i<4; i++) {
            int nx = sn.find(QChar(i==3 ? '/' : '.'),x);
            auth_peer = (auth_peer<<8)|sn.mid(x,nx-x).toInt();
            x = nx+1;
        }
        uint n = (uint)sn.find(' ',x)-x;
        auth_peer_bits = sn.mid(x,n).toInt();
    }
}

void MultiauthConfig::loadUsers()
{
    QFile passwd("/etc/passwd");
    if ( passwd.open(IO_ReadOnly) ) {
        QTextStream t( &passwd );
        QString s;
        QStringList account;
        while ( !t.eof() ) {
            account = QStringList::split(':',t.readLine());
            // Hide disabled accounts and some special accounts
            if (*account.at(1)!="*" && *account.at(0)!="ppp" && *account.at(0)!="messagebus") {

                m_loginWidget->userlist->insertItem(*account.at(0));
                // Highlight this item if it is set to m_loginWidget->autologinToggle
                if ( *account.at(0) == autoLoginName)
                    m_loginWidget->userlist->setCurrentItem(m_loginWidget->userlist->count()-1);
            }
        }
        passwd.close();
    }

}

void MultiauthConfig::toggleAutoLogin(bool val)
{
    autoLogin=val;
    m_loginWidget->userlist->setEnabled(val);
    // if autoLogin is true, we will set by default the login currently visible in the userlist
    if (autoLogin)
        autoLoginName=m_loginWidget->userlist->currentText();
}




void MultiauthConfig::setSyncNet(const QString& sn)
{
    int auth_peer,auth_peer_bits;
    parseNet(sn,auth_peer,auth_peer_bits);
    selectNet(auth_peer,auth_peer_bits,FALSE);
}

void MultiauthConfig::changeLoginName( int idx )
{
    autoLoginName = m_loginWidget->userlist->text(idx);;
    updateGUI();
}

/// \todo do implement that? who? how?
bool MultiauthConfig::telnetAvailable() const
{
    return FALSE;
}

/// \todo do implement that? who? how?
bool MultiauthConfig::sshAvailable() const
{
    return FALSE;
}

