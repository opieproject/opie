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
 * \brief Create an OPluginItem
 *
 * Create a Plugin Item with all information.
 *
 * @param name The if available translated Name
 * @param path The path to the plugin
 * @param pos  The position of the plugin if used for sorting
 *
 */
OPluginItem::OPluginItem( const QString& name, const QString& path, int pos )
    : m_name( name ), m_path( path ), m_pos( pos ) {
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
    if ( m_path != r.m_path ) return false;
    return true;
}

bool OPluginItem::operator!=( const OPluginItem& r )const{
    return !( *this == r );
}

/**
 * return the name of this Plugin
 */
QString OPluginItem::name()const {
    return m_name;
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
      m_isSorted( isSorted )
{
    setPluginDir( QPEApplication::qpeDir() + "/plugins/"+name );
    readConfig();
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
    Internal::OPluginLibraryHolder::self()->deref( m_library.take( iface ) );
}

/**
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
 * Enter or leave SafeMode
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
 * Return a List of Plugins for a dir and add positions and remove disabled.
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


    OConfig cfg( m_dir+"odpplugins" );
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
    QStringList::Iterator it;
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        QString str = unlibify( *it );
        OPluginItem item( str, _dir + "/" + *it );

        bool ex = excludedMap.contains( str );
        /*
         * if disabled but we should show all assign a -2
         * else continue because we don't want to add the item
         * else if sorted we assign the right position
         */
        if ( ex && !disabled)
            item.setPosition( -2 );
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



}
}
