/*
 * LGPLv2 or later
 * zecke@handhelds.org
 */

#include "opluginloader.h"
#include "oconfig.h"

#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qdict.h>
#include <qtl.h>
#include <qfile.h>

#include <stdlib.h>



namespace Opie {
namespace Core {
namespace Internal {
struct OPluginLibraryHolder {
    static OPluginLibraryHolder *self();
    QLibrary *ref( const QString& );
    void deref( QLibrary* );
private:

    OPluginLibraryHolder();
    ~OPluginLibraryHolder();
    QDict<QLibrary> m_libs;
    static OPluginLibraryHolder* m_self;
};

    OPluginLibraryHolder* OPluginLibraryHolder::m_self  = 0;
    OPluginLibraryHolder* OPluginLibraryHolder::self() {
        if ( !m_self )
            m_self = new OPluginLibraryHolder;

        return m_self;
    }

    OPluginLibraryHolder::OPluginLibraryHolder() {}
    OPluginLibraryHolder::~OPluginLibraryHolder() {}

    /*
     * We do simple ref counting... We will add the QLibrary again
     * and again to the dictionary and on deref we will pop and pop it
     * until there are no more library and we will unload and delete the library
     * luckily dlopen does some ref counting as well so we don't need
     * to hack QPEApplication
     */
    QLibrary* OPluginLibraryHolder::ref(const QString& str) {
        QLibrary *lib = m_libs[str];

        /* if not in the dict try to load it */
        if ( !lib ) {
            lib = new QLibrary( str, QLibrary::Immediately );
            if ( !lib->isLoaded() ) {
                delete lib;
                return 0l;
            }
        }

        /* now refcount one up */
        m_libs.insert( str, lib );
        return lib;
    }

    /*
     * 'unshadow' the items until we're the last then unload and delete
     */
    void OPluginLibraryHolder::deref( QLibrary* lib ) {
        if ( !lib )
            return;

        QString str = lib->library();
        /* no need to check if the lib was inserted or such */
        (void) m_libs.take( str );
        if ( !m_libs[str] ) {
            lib->unload();
            delete lib;
        }
    }
}

/**
 * We want values with 30,29,28 at the beginning of the list
 */

bool operator<( const OPluginItem& l, const OPluginItem& r ) {
    return l.position() > r.position();
}

bool operator>( const OPluginItem& l, const OPluginItem& r ) {
    return l.position() < r.position();
}

bool operator<=( const OPluginItem& l, const OPluginItem& r ) {
    return l.position() >=  r.position();
}

/**
 * \brief Creates an Empty OPluginItem
 *
 * create an empty pluginitem. Position is set to -1 and the
 * other things are used with the default ctors
 */
OPluginItem::OPluginItem()
    : m_pos( -1 ) {
}

/**
 * @todo Create Internal name so we can have the plugin names translated. Or only for the gui? I'm not yet sure
 * \brief Create an OPluginItem
 *
 * Create a Plugin Item with all information. If you for some reasons
 * need to create your own OPluginItems make sure that 'name' is always the same
 * as it if used for positions and exclude list.
 *
 * @param name The if available translated Name
 * @param path The path to the plugin must be absolute.
 * @param b If the OPluginItem is enabled or not
 * @param pos  The position of the plugin if used for sorting
 *
 */
OPluginItem::OPluginItem( const QString& name, const QString& path, bool b, int pos )
    : m_name( name ), m_path( path ), m_enabled( b ), m_pos( pos )
{}

/**
 * \brief simple d'tor
 */
OPluginItem::~OPluginItem() {
}

/**
 * \brief Test if this Item is Empty and does not represent a loadable plugin
 * Test if a OPluginItem is empty. A OPluginItem is empty if name and path are empty
 * ,pos equals -1 and the item is disabled
 *
 * @see QString::isEmpty()
 *
 */
bool OPluginItem::isEmpty()const {
    if ( m_pos != -1 )       return false;
    if ( m_enabled )         return false;
    if ( !m_name.isEmpty() ) return false;
    if ( !m_path.isEmpty() ) return false;

    return true;
}

/**
 * \brief test equality
 * operator to test equalness of two OPluginItem
 */
bool OPluginItem::operator==( const OPluginItem& r )const{
    if ( m_pos     != r.m_pos    ) return false;
    if ( m_enabled != r.m_enabled) return false;
    if ( m_name    != r.m_name   ) return false;
    if ( m_path    != r.m_path   ) return false;
    return true;
}

/**
 * \brief test non equality
 * operator to test non-equalness of two OPluginItem
 */
bool OPluginItem::operator!=( const OPluginItem& r )const{
    return !( *this == r );
}

/**
 * \brief returns the name of the plugin
 * return the name of this Plugin
 */
QString OPluginItem::name()const {
    return m_name;
}

/**
 * \brief return the path of the plugin
 */
QString OPluginItem::path()const {
    return m_path;
}

/**
 * \brief Return if this item is enabled.
 */
bool OPluginItem::isEnabled()const {
    return m_enabled;
}

/**
 * \brief return the position of a plugin.
 * return the position of the item
 * -1 is the default value and means normally that the whole items are unsorted.
 * Higher numbers belong to an upper position. With plugins with the postions 20,19,5,3
 * the item with pos  20 would be the first in the list returned by the OGenericPluginLoader
 *
 * @see OGenericPluginLoader::allAvailable
 * @see OGenericPluginLoader::filtered
 */
int OPluginItem::position()const{
    return m_pos;
}

/**
 * \brief set the name of a plugin
 * Set the name of the Plugin Item
 * @param name
 */
void OPluginItem::setName(  const QString& name ) {
    m_name = name;
}

/**
 * \brief set the path of a plugin
 * Set the path of Plugin Item. The path must be absolute.
 * @param name The path of the plugin
 */
void OPluginItem::setPath( const QString& name ) {
    m_name = name;
}

/**
 * \brief enable or disable the to load attribute
 * Set the Enabled attribute. Such changes won't be saved. If you want to save it
 * use a OPluginManager to configure your plugins manually or Opie::Ui::OPluginConfig
 * for a graphical frontend.
 *
 * @param enabled Enable or Disable the Enabled Attribute
 */
void OPluginItem::setEnabled(  bool enabled ) {
    m_enabled = enabled;
}

/**
 * \brief Set the position.
 * Set the position
 * @param pos The position
 *
 * @see position()
 */
void OPluginItem::setPosition( int pos ) {
    m_pos = pos;
}



/**
 * \brief create a PluginLoader
 *
 * Create a PluginLoader autoDelete is set to false
 *
 * \code
 * Opie::Core::OGenericPluginLoader loader("myapp-plugin");
 * Opie::Core::OPluginItem::List  lst = loader.filtered();
 * for(Opie::Core::OPluginItem::List::Iterator it = lst.begin(); it!=lst.end();++it){
 *    MyIface* iface = static_cast<MyIface*>(loader.load(*it,IID_MyIface));
 * }
 * \endcode
 *
 * \code
 * Opie::Core::OGenericPluginLoader loader("myapp-plugin");
 * Opie::Core::OPluginItem::List  lst = loader.filtered();
 * for(Opie::Core::OPluginItem::List::Iterator it = lst.begin(); it!=lst.end();++it){
 *    MyIface* iface = static_cast<MyIface*>(loader.load(*it,IID_MyIface));
 * }
 * ...
 * loader.clear();
 *
 * \endcode
 *
 * @param name The name of the plugin directory.
 * @param isSorted Tell the PluginLoader if your Plugins are sorted
 */
OGenericPluginLoader::OGenericPluginLoader( const QString& name,  bool isSorted)
    : m_dir( name ), m_autoDelete( false ), m_isSafeMode( false ),
      m_isSorted( isSorted )
{
    setPluginDir( QPEApplication::qpeDir() + "/plugins/"+name );
    readConfig();
}


/**
 * \brief simple d'tor that cleans up depending on autoDelete
 *
 * calls clear if autoDelete is true. This will release all interfaces
 * and remove the library from this process if the refcount falls to zero
 */
OGenericPluginLoader::~OGenericPluginLoader() {
    if ( m_autoDelete )
        clear();
}

/**
 * \brief Enable or disable autoDelete on destruction
 *
 * enable autoDelete. This will call clear on the d'tor
 *
 * @see ~OGenericPluginLoader
 * @see clear()
 */
void OGenericPluginLoader::setAutoDelete( bool t ) {
    m_autoDelete = t;
}

/**
 * \brief See if autoDelete is enabled.
 */
bool OGenericPluginLoader::autoDelete()const{
    return m_autoDelete;
}

/**
 * \brief unload all loaded Plugins
 *
 * This will unload all returned QUnknownInterfaces by load. Unload
 * will be called.
 */
void OGenericPluginLoader::clear() {
    QPtrDictIterator<QLibrary> it( m_library );
    for ( ;it.current(); )
        unload( static_cast<QUnknownInterface*>( it.currentKey() ) );
}

/**
 * \brief unload the Plugin and the accompanied Resources.
 *
 * This will take the iface from the internal QPtrDict, Release it,
 * and deref the libray used.
 * The visibility depends on the QPtrDict.
 * @see QPtrDict::insert
 */
void OGenericPluginLoader::unload( QUnknownInterface* iface ) {
    if ( !iface )
        return;

    iface->release();
    Internal::OPluginLibraryHolder::self()->deref( m_library.take( iface ) );
}

/**
 * \brief The name of the plugins.
 *
 * Return the name/type you specified in the constructor.
 * This is at least used by the OPluginManager to find the right config
 */
QString OGenericPluginLoader::name()const {
    return m_dir;
}


/**
 * \brief See if loading of a plugin segfaulted
 * This tells you
 * if by previous tries to load, loading crashed your application.
 * If isInSafeMode you can use the GUI to configure the plugins prior to loading
 *
 * @return true if prior loading failed
 */
bool OGenericPluginLoader::isInSafeMode()const {
    return m_isSafeMode;
}


/**
 * \brief Return all Plugins found in the plugins dirs.
 * Return the list of all available plugins. This will go through all plugin
 * directories and search for your type of plugins ( by subdir )
 *
 * @param sorted Tell if you want to have the positions sorted. This only makes sense if you
 */
OPluginItem::List OGenericPluginLoader::allAvailable( bool sorted )const {
    OPluginItem::List lst;
    for ( QStringList::ConstIterator it = m_plugDirs.begin(); it != m_plugDirs.end(); ++it )
        lst += plugins(  *it, sorted, false );

    if ( sorted )
        qHeapSort( lst );
    return lst;
}

/**
 * \brief Return only the enabled plugins
 * Return only activated plugins.
 *
 * @param sorted If the list should be sorted
 */
OPluginItem::List OGenericPluginLoader::filtered( bool sorted )const {
    OPluginItem::List lst;
    for ( QStringList::ConstIterator it = m_plugDirs.begin(); it != m_plugDirs.end(); ++it )
        lst += plugins(  *it, sorted, true );

    if ( sorted )
        qHeapSort( lst );
    return lst;
}


/**
 * \brief Load a OPluginItem for the specified interface
 * This will open the resource of the OPluginItem::path() and then will query
 * if the Interface specified in the uuid is available and then will manage the
 * resource and Interface.
 *
 * @param item The OPluginItem that should be loaded
 * @param uuid The Interface to query for
 *
 * @return Either 0 in case of failure or the Plugin as QUnknownInterface*
 */
QUnknownInterface* OGenericPluginLoader::load( const OPluginItem& item, const QUuid& uuid) {
    /*
     * Check if there could be a library
     */
    QString pa = item.path();
    if ( pa.isEmpty() )
        return 0l;

    /*
     * See if we get a library
     * return if we've none
     */
    setSafeMode( pa, true );
    QLibrary *lib = Internal::OPluginLibraryHolder::self()->ref( pa );
    if ( !lib ) {
        setSafeMode();
        return 0l;
    }

    /**
     * try to load the plugin and just in case initialize the pointer to a pointer again
     */
    QUnknownInterface*  iface=0;
    if ( lib->queryInterface(  uuid,  &iface ) == QS_OK ) {
        installTranslators(pa.left( pa.find(".")));
        m_library.insert( iface, lib );
    }else
        iface = 0;

    setSafeMode();

    return iface;
}

/**
 * @internal and reads in the safe mode
 */
void OGenericPluginLoader::readConfig() {


/* read the config for SafeMode */
    OConfig conf(  m_dir + "-odpplugins" );
    conf.setGroup( "General" );
    m_isSafeMode = conf.readBoolEntry( "SafeMode", false );
}

/**
 * @internal Enter or leave SafeMode
 */
void OGenericPluginLoader::setSafeMode(const QString& str, bool b) {
    OConfig conf(  m_dir + "-odpplugins" );
    conf.setGroup( "General" );
    conf.writeEntry( "SafeMode", b );
    conf.writeEntry( "CrashedPlugin", str );
}

/**
 * @internal
 *
 * Set the List of Plugin Dirs to lst. Currently only QPEApplication::qpeDir()+"/plugins/"+mytype
 * is used as plugin dir
 */
void OGenericPluginLoader::setPluginDirs( const QStringList& lst ) {
    m_plugDirs = lst;
}

/**
 *
 * @internal
 * Set the Plugin Dir to str. Str will be the only element in the list of plugin dirs
 */
void OGenericPluginLoader::setPluginDir( const QString& str) {
    m_plugDirs.clear();
    m_plugDirs.append( str );
}


/**
 * @internal
 */
bool OGenericPluginLoader::isSorted()const{
    return m_isSorted;
}

/*
 * make libfoo.so.1.0.0 -> foo on UNIX
 * make libfoo.dylib    -> foo on MAC OS X Unix
 * windows is obviously missing
 */
/**
 * @internal
 */
QString OGenericPluginLoader::unlibify( const QString& str ) {
    QString st = str.mid( str.find( "lib" )+3 );
#ifdef Q_OS_MACX
    return st.left( st.findRev( ".dylib" ) );
#else
    return st.left( st.findRev( ".so" ) );
#endif
}

/**
 * @internal
 *
 * \brief method to return available plugins. Internal and for reeimplementations
 *
 *Return a List of Plugins for a dir and add positions and remove disabled.
 * If a plugin is on the excluded list assign position -2
 *
 * @param dir The dir to look in
 * @param sorted Should positions be read?
 * @param disabled Remove excluded from the list
 */
OPluginItem::List OGenericPluginLoader::plugins( const QString& _dir, bool sorted, bool disabled )const {
#ifdef Q_OS_MACX
    QDir dir( _dir, "lib*.dylib" );
#else
    QDir dir( _dir, "lib*.so" );
#endif

    OPluginItem::List lst;

    /*
     * get excluded list and then iterate over them
     * Excluded list contains the name
     * Position is a list with 'name.pos.name.pos.name.pos'
     *
     * For the look up we will create two QMap<QString,pos>
     */
    QMap<QString, int> positionMap;
    QMap<QString, int> excludedMap;


    OConfig cfg( m_dir+"-odpplugins" );
    cfg.setGroup( _dir );


    QStringList excludes = cfg.readListEntry( "Excluded", ',' );
    for ( QStringList::Iterator it = excludes.begin(); it != excludes.end(); ++it )
        excludedMap.insert( *it, -2 );

    if ( m_isSorted ) {
        QStringList pos =  cfg.readListEntry( "Positions", '.' );
        QStringList::Iterator it = pos.begin();
        while ( it != pos.end() )
            positionMap.insert(  *it++, (*it++).toInt() );
    }




    QStringList list = dir.entryList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        QString str = unlibify( *it );
        OPluginItem item( str, _dir + "/" + *it );

        bool ex = excludedMap.contains( str );
        /*
         * if disabled but we should show all mark it as disabled
         * else continue because we don't want to add the item
         * else if sorted we assign the right position
         */
        if ( ex && !disabled)
            item.setEnabled( false );
        else if ( ex && disabled )
            continue;
        else if ( sorted )
            item.setPosition( positionMap[str] );

        lst.append( item );
    }

    return lst;
}

/**
 * @internal generate a list of languages from $LANG
 */
QStringList OGenericPluginLoader::languageList() {
    if ( m_languages.isEmpty() ) {
        /*
         * be_BY.CP1251 We will add, be_BY.CP1251,be_BY,be
         * to our list of languages.
         */
        QString str = ::getenv( "LANG" );
        m_languages += str;
        int pos = str.find( '.' );

        if ( pos > 0 )
            m_languages += str.left( pos );

        int n_pos = str.find( '_' );
        if ( pos > 0 && n_pos >= pos )
            m_languages += str.left( n_pos );

    }
    return m_languages;
}

/**
 * @internal
 * Tries to install languages using the languageList for the type
 */
void OGenericPluginLoader::installTranslators(const QString& type) {
    QStringList lst = languageList();

    /*
     * for each language and maybe later for each language dir...
     * try to load a Translator
     */
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QTranslator* trans = new QTranslator(  qApp );
        QString tfn = QPEApplication::qpeDir()+"/i18n/" + *it + "/" + type + ".qm" ;

        /*
         * If loaded then install else clean up and don't leak
         */
        if ( trans->load( tfn ) )
            qApp->installTranslator( trans );
        else
            delete trans;
    }
}

/**
 * \brief Simple c'tor.
 *
 * Simple C'tor same as the one of the base class. Additional this
 * class can cast for you if you nee it.
 *
 *
 * @param name The name of your plugin class
 * @param sorted If plugins are sorted
 *
 * @see OGenericPluginLoader
 */
OPluginLoader::OPluginLoader( const QString& name,  bool sorted )
    : OGenericPluginLoader( name, sorted )
{
}

/**
 * d'tor
 * @see OGenericPluginLoader::~OGenericPluginLoader
 */
OPluginLoader::~OPluginLoader() {
}

/**
 * \brief C'Tor using a OGenericPluginLoader
 * The C'tor. Pass your OGenericPluginLoader to manage
 * OGenericPluginLoader::allAvailable plugins.
 *
 *
 * @param loader A Pointer to your OGenericPluginLoader
 * @param name The name
 */
OPluginManager::OPluginManager(  OGenericPluginLoader* loader)
    : m_loader( loader ), m_isSorted( false )
{
}

/**
 * \brief Overloaded c'tor using a List of Plugins and a type name
 * Overloaded Constructor to work with a 'Type' of plugins
 * and a correspending list of those. In this case calling load
 * is a no operation.
 *
 * @param name The name of your plugin ('today','inputmethods','applets')
 * @param lst A List with plugins of your type to manage
 * @param isSorted If the List should be treated sorted
 */
OPluginManager::OPluginManager( const QString& name, const OPluginItem::List& lst,  bool isSorted)
    : m_loader( 0l ), m_cfgName( name ), m_plugins( lst ), m_isSorted( isSorted )
{
}

/**
 * \brief A simple d'tor
 */
OPluginManager::~OPluginManager() {
}

/**
 * \brief Return the OPluginItem where loading is likely to have crashed  on.

 * Return the Item that made the OGenericPluginLoader crash
 * the returned OPluginItem could be  empty if no crash occured
 * which should apply most of the time. It could also be empty if the crashed
 * plugin is not in the current list of available/managed plugins
 *
 * @see OPluginItem::isEmpty
 * @return OPluginItem that crashed  the loader
 */
OPluginItem OPluginManager::crashedPlugin()const {
    return m_crashed;
}

/**
 * \brief Return a list of plugins that are managed by this OPluginManager
 *
 * Return the list of managed plugins. This could either result
 * from passing a OGenericPluginLoader and calling load or by
 * giving name and a list of plugins.
 */
OPluginItem::List OPluginManager::managedPlugins()const {
    return m_plugins;
}

/**
 * \brief Set the position of the items
 *
 * Replace the OPluginItem  with the name and path and this way
 * apply the new position. The search is linear and this way O(n/2)
 * You still need to call save()  to make your changes effective. After saving
 * a call to OGenericPluginLoader::filtered() returns the newly configured order and items
 *
 * @param item The  OPluginItem to be replaced internall
 *
 */
void OPluginManager::setPosition( const OPluginItem& item) {
    replace( item );
}

/**
 * \brief Enable the item specified as argument
 *
 * This will make sure that OPluginItem::setEnabled is called and then will replace
 * the item with one that matches name and path internally.
 * @see setPosition
 *
 * @param the Item to enable
 */
void OPluginManager::enable(  const OPluginItem& item ) {
    setEnabled( item, true );
}

/**
 * \brief disable the Item.
 *
 * Disable the OPluginItem. Same applies as in
 * @see setPosition and @see enable
 *
 * @param item Item to disable
 */
void OPluginManager::disable( const OPluginItem& item) {
    setEnabled( item, false );
}

/**
 * \brief Enable or disable the OPluginItem.
 * Depending on the value of the parameter this will either disable
 * or enable the pluginitem.
 * Beside that same as in @see disable,  @see enable, @see setPosition
 * applies.
 *
 * @param _item The OPluginItem to enable or to disable.
 * @param b Enable or disable the plugin.
 *
 */
void OPluginManager::setEnabled( const OPluginItem& _item, bool b ) {
    OPluginItem item = _item;
    item.setEnabled( b );
    replace( item );
}

/**
 * \brief Load necessary information after constructing the object
 * If you speified  a OGenericPluginLoader you need to call this method
 * so that this manager knows what to manage and have a right value for \sa crashedPlugin
 * For the name and the list of plugins this does only try to find out the crashed plugin
 */
void OPluginManager::load() {
    OConfig cfg( configName() );
    cfg.setGroup(  "General" );
    QString crashedPath = cfg.readEntry( "CrashedPlugin" );

    /* if we've a loader this applies if we were called from the first part */
    if ( m_loader )
        m_plugins = m_loader->allAvailable( m_loader->isSorted() );

    /*  fast and normal route if we did not crash... */
    if ( crashedPath.isEmpty() )
        return;

    /* lets try to find the plugin path and this way the associated item */
    for ( OPluginItem::List::Iterator it = m_plugins.begin(); it != m_plugins.end(); ++it )
        if (  (*it).path() ==  crashedPath ) {
            m_crashed = *it;
            break;
        }
}


/**
 * \brief Save the values and this way make it available.
 *
 * Save the current set of data. A call to @see OGenericPluginLoader::filtered
 * now would return  your saved changes.
 */
void OPluginManager::save() {
    QMap<QString, QStringList> excluded;  // map for path to excluded name
    QMap<QString, QStringList> positions; // if positions matter contains splitted up by dirs
    bool sorted = m_loader ? m_loader->isSorted() : m_isSorted;

    /*
     * We will create some maps for the  groups to include positions a
     */
    for ( OPluginItem::List::Iterator it = m_plugins.begin(); it != m_plugins.end(); ++it ) {
        OPluginItem item = *it;
        QString path = QFileInfo(  item.path() ).filePath();
        if ( sorted ) {
            positions[path].append( item.name() );
            positions[path].append( QString::number( item.position() ) );
        }

        if ( !item.isEnabled() )
            excluded[path].append( item.name() );
    }

/*
 * The code below wouldn't work because we can't delete groups/keys from the config
 * ### for ODP   make Config right!
 */
//    if ( excluded.isEmpty() && positions.isEmpty() ) return;
    /*
     * Now safe for each path
     */
    OConfig cfg( configName() );

    /* safe excluded items */
    for ( QMap<QString, QStringList>::Iterator it = excluded.begin(); it != excluded.end(); ++it ) {
        OConfigGroupSaver saver( &cfg, it.key() );
        cfg.writeEntry("Excluded", it.data(), ',' );
    }

    /*  safe positions we could also see if positions.contains(path) and remove/write in the above loop
     * ### Write a Test Suite that can profile these runs...
     */
    for ( QMap<QString, QStringList>::Iterator it = positions.begin(); it != positions.end(); ++it ) {
        OConfigGroupSaver saver( &cfg, it.key() );
        cfg.writeEntry("Positions", it.data(), '.' );
    }
}

/**
 * @internal
 */
QString OPluginManager::configName()const {
    QString str = m_loader ? m_loader->name() : m_cfgName;
    return str + "-odpplugins";
}

/**
 * @internal.. replace in m_plugins by path... this is linear search O(n/2)
 */
void OPluginManager::replace( const OPluginItem& item ) {
    OPluginItem _item;

    /* for all plugins */
    for ( OPluginItem::List::Iterator it=m_plugins.begin();it != m_plugins.end(); ++it ) {
        _item = *it;
        /* if path and name are the same we will remove, readd and return */
        if ( _item.path() == item.path() &&
             _item.name() == item.name() ) {
            it = m_plugins.remove( it );
            m_plugins.append( item );
            return;
        }

    }
}

}
}
