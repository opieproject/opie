/*
 * LGPLv2 or later
 * zecke@handhelds.org
 */

#include "opluginloader.h"

#include <stdlib.h>



namespace Opie {
namespace Core {
namespace Internal {
struct OPluginLibraryHolder {
    static OPluginLibraryHolder *self();
    QLibrary *getLibrary( const QString& );
    void putLibrary( QLibrary* );
private:
    OPluginLibraryHolder();
    ~OPluginLibraryHolder();
    OPluginLibraryHolder* m_self;

};
}

}}
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
 * \brief Create an OPluginItem
 *
 * Create a Plugin Item with all information.
 *
 * @param name The if available translated Name
 * @param conf The name of the config group
 * @param path The path to the plugin
 * @param pos  The position of the plugin if used for sorting
 *
 */
OPluginItem::OPluginItem( const QString& name, const QCString& conf,
                          const QString& path, int pos = -1 )
    : m_name( name ), m_conf( conf ), m_path( path ), m_pos( pos ) {
}

/**
 * \brief simple d'tor
 */
OPluginItem::~OPluginItem() {
}

/**
 * operator to test equalness of two OPluginItem
 */
bool OPluginItem::operator==( const OPluginItem& r )const{
    if ( m_pos  != r.m_pos  ) return false;
    if ( m_name != r.m_name ) return false;
    if ( m_conf != r.m_conf ) return false;
    if ( m_path != r.m_path ) return false;
    return true;
}

bool OPluginItem::operator!=( const OPluginItem& r ) {
    return !( *this == r );
}

/**
 * return the name of this Plugin
 */
QString OPluginItem::name()const {
    return m_name;
}

/**
 * return the config key
 */
QCString OPluginItem::configKey()const{
    return m_conf;
}

/**
 * return the path of the plugin
 */
QString OPluginItem::path()const {
    return m_path;
}

int OPluginItem::position()const{
    return m_pos;
}

/**
 * Set the name of the Plugin Item
 * @param name
 */
void OPluginItem::setName(  const QString& name ) {
    m_name = name;
}

/**
 * Set the config key
 * @param key The config key/group of this plugin
 */
void OPluginItem::setConfigKey( const QCString& conf ) {
    m_conf = conf;
}

/**
 * Set the path of Plugin Item
 * @param name The path of the plugin
 */
void OPluginItem::setPath( const QString& name ) {
    m_name = name;
}

/**
 * Set the position
 * @param pos The position
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
      m_isSorted( isSorted ), m_readConfig( false )
{
    setPluginDir( QPEApplication::qpeDir() + "/plugins/"+name );
}


/**
 * calls clear if autoDelete is true. This will release all interfaces
 * and remove the library from this process if the refcount falls to zero
 */
OGenericPluginLoader::~OGenericPluginLoader() {
    if ( m_autoDelete )
        clear();
}

/**
 * enable autoDelete. This will call clear on the d'tor
 *
 * @see ~OGenericPluginLoader
 * @see clear()
 */
void OGenericPluginLoader::setAutoDelete( bool t ) {
    m_autoDelete = t;
}

/**
 * see if autoDelet is enabled
 */
bool OGenericPluginLoader::autoDelete()const{
    return m_autoDelete;
}

/**
 * This will unload all returned QUnknownInterfaces by load. Unload
 * will be called.
 */
void OGenericPluginLoader::clear() {
    QPtrDictIterator<QLibrary> it( m_library );
    for ( ;it.current(); )
        unload( static_cast<QUnknownInterface*>( it.currentKey() ) );
}

/**
 * This will take the iface from the internal QPtrDict, Release it,
 * and deref the libray used.
 * The visibility depends on the QPtrDict.
 * @see QPtrDict::insert
 */
void OGenericPluginLoader::unload( QUnknownInterface* iface ) {
    if ( !iface )
        return;

    iface->release();
    OPluginLibraryHolder::self()->deref( m_library.take( iface ) );
}

/**
 * This tells you
 * if by previous tries to load, loading crashed your application.
 * If isInSafeMode you can use the GUI to configure the plugins prior to loading
 *
 * @return true if prior loading failed
 */
bool OGenericPluginLoader::isInSafeMode()const {
    if ( !m_readConfig )
        readConfig();
    return m_isSafeMode;
}

/**
 * return if the plugin list is sorted.
 */
bool OGenericPluginLoader::isSorted()const {
    if ( !m_readConfig )
        readConfig();
    return m_isSorted;
}

/**
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
 *
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
    setSafeMode( true );
    QLibrary *lib = OPluginLibraryHolder::self()->getLibrary( pa );
    if ( !lib ) {
        setSafeMode();
        return 0l;
    }

    /**
     * try to load the plugin and just in case initialize the pointer to a pointer again
     */
    QUnknownInterface**  iface = 0;
    if ( lib->queryInterface(  uuid,  iface ) == QS_OK ) {
        installTranslators(pa.left( pa.find(".")));
        m_library.insert( *iface, lib );
    }else
        *iface = 0;

    setSafeMode();

    return *iface;
}

void OGenericPluginLoader::readConfig() {
    m_readConfig = true;

    /* read the config for SafeMode */
    OConfig conf(  m_name + "-odpplugins" );
    conf.setGroup( "General" );
    m_isSafeMode = conf.readBoolEntry( "SafeMode", false );
}

void OGenericPluginLoader::setSafeMode(bool b) {
    OConfig conf(  m_name + "-odpplugins" );
    conf.setGroup( "General" );
    conf.writeEntry( "SafeMode", b );
}

void OGenericPluginLoader::setPluginDirs( const QStringList& lst ) {
    m_plugDirs = lst;
}

void OGenericPluginLoader::setPluginDir( const QString& str) {
    m_plugDirs.clear();
    m_plugDirs.append( str );
}

bool &OGenericPluginLoader::isSafeMode()const {
    return m_isSafeMode;
}

bool &OGenericPluginLoader::isSorted()const {
    return m_isSorted;
}

OPluginItem::List OGenericPluginLoader::plugins( const QString& dir, bool sorted, bool disabled ) {
#ifdef Q_OS_MACX
    QDir dir( dir, "lib*.dylib" );
#else
    QDir dir( dir, "lib*.so" );
#endif
    /*
     * get excluded list and then iterate over them
     */
    OConfig cfg( m_name+"odpplugins" );
    cfg.setGroup( dir );
    QStringList excludes = cfg.readListEntry( "Excluded" );

    QStringList list = dir.entryList();
//    for (

}


QStringList OGenericPluginLoader::languageList() {
    if ( languageList.isEmpty() ) {
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

void OGenericPluginLoader::installTranslators(const QString& type) {
    QStringList lst = languageList();
    for ( QStringList::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        QTranslator* trans = new QTranslator(  qApp );
        QString tfn = QPEApplication::qpeDir()+"/i18n/" + lang + "/" + type + ".qm" ;
        if ( trans->load( tfn ) )
            qApp->installTranslator( trans );
        else
            delete trans;
    }
}



}
}
