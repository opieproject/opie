/*
 * LGPLv2 or later
 * zecke@handhelds.org
 */
#ifndef ODP_CORE_OPLUGIN_LOADER_H
#define ODP_CORE_OPLUGIN_LOADER_H

#include <qpe/qlibrary.h>

#include <qptrdict.h>
#include <qstringlist.h>

namespace Opie {
namespace Core {
class OConfig;
namespace Internal {
class OPluginLibraryHolder;
}

template class QPtrDict<QLibrary>;

/**
 * \brief A small item representing the Plugin Information
 * This class contains the information about a Plugin. It contains
 * a translated name if available to the system, a config key,
 * and the path location.
 *
 * @since 1.2
 *
 */
class OPluginItem {
public:
    typedef QValueList<OPluginItem> List;
    OPluginItem();
    OPluginItem( const QString& name, const QString& path, bool enabled = true, int pos = -1 );
    ~OPluginItem();

    bool isEmpty()const;

    bool operator==( const OPluginItem& )const;
    bool operator!=( const OPluginItem& )const;


    QString  name()const;
    QString  path()const;
    bool isEnabled()const;
    int position()const;

    void setName( const QString& );
    void setPath( const QString& );
    void setEnabled(  bool );
    void setPosition( int );

private:
    QString m_name;
    QString m_path;
    bool  m_enabled : 1;
    int m_pos;
    struct Private;
    Private *d;
};

/**
 * \brief A generic class to easily load and manage plugins
 *
 * This is the generic non sepcialised loader for plugins. Normally
 * you would prefer using the OPluginLoader directly. This class
 * exists to minimize the application binary size due the usage
 * of templates in the specialized API
 *
 * @since 1.2
 * @see OPluginLoader
 */
class OGenericPluginLoader {
public:
    typedef OPluginItem::List List;
    OGenericPluginLoader( const QString &name, bool isSorted = false );
    virtual ~OGenericPluginLoader();

    void setAutoDelete( bool );
    bool autoDelete()const;
    void clear();

    QString name()const;
    bool isSorted()const;
    bool isInSafeMode()const;


    List  allAvailable(bool sorted = false )const;
    List  filtered(bool sorted = false )const;


    virtual QUnknownInterface* load( const OPluginItem& item, const QUuid& );
    virtual void unload( QUnknownInterface* );

protected:
    friend class OPluginManager; // we need the static unlibify
    void readConfig();
    virtual List plugins( const QString& dir, bool sorted, bool disabled )const;
    void setPluginDirs( const QStringList& );
    void setPluginDir( const QString& );
    void setSafeMode(const QString& app = QString::null,  bool b = false);
    static QString unlibify( const QString& str );
private:
    QStringList languageList();
    void installTranslators(const QString& type);
    QString m_dir;
    QStringList m_plugDirs;
    QStringList m_languages;
    bool m_autoDelete : 1;
    bool m_isSafeMode : 1;
    bool m_isSorted   : 1;
    QPtrDict<QLibrary> m_library;

    struct Private;
    Private* d;
};

/**
 * \brief The class to load your QCOM+ plugins
 *
 * This class takes care of activation and even the order
 * if you need it. It is normally good to place a .directory file
 * into your plugin directory if you need order of activation.
 *
 * You'll create the OPluginLoader and then use it to load the filtered
 *  plugins.
 *
 * There is also a GUI for the configuration and a Manager to write the
 * mentioned .directory file
 *
 * On crash the safe mode is activated for the next run. You can then decide
 * if you want to load plugins or come up with the  Configuration on
 * next start yourself then.
 *
 * @since 1.2
 */
class OPluginLoader : public OGenericPluginLoader {
public:
    OPluginLoader( const QString& name, bool sorted = false );
    virtual ~OPluginLoader();

    template<class IFace>
    IFace* load( const OPluginItem& item, const QUuid& );
};

/**
 * \brief A class to manage order and activation of plugins
 *
 * Manage order and activation. This is used by the Opie::Ui::OPluginConfig
 * This class controls the activation and order of plugins depending
 * on the OPluginLoader you supply.
 * You must call load() and save after construnction an instance
 *
 * @see Opie::Ui::OPluginConfig
 *
 */
class OPluginManager {
public:
    OPluginManager( OGenericPluginLoader* );
    OPluginManager( const QString& name, const OPluginItem::List&, bool isSorted = false );
    virtual ~OPluginManager();

    OPluginItem crashedPlugin()const;

    OPluginItem::List managedPlugins()const;

    void setPosition( const OPluginItem& );
    void enable( const OPluginItem& );
    void disable( const OPluginItem& );
    void setEnabled( const OPluginItem&, bool = true);

    virtual void load();
    virtual void save();

protected:
    QString configName()const;
    void replace( const OPluginItem& );
private:
    OGenericPluginLoader *m_loader;
    QString m_cfgName;
    OPluginItem::List m_plugins;
    OPluginItem m_crashed;
    bool m_isSorted : 1;
};


/**
 * This is a template method allowing you to safely cast
 * your load function
 *
 * \code
 * MyTypePlugin *plug = load->load<MyTypePlugin>( item, IID_MyPlugin );
 * \endcode
 *
 */
template<class IFace>
IFace* OPluginLoader::load( const OPluginItem& item, const QUuid& uid ) {
    return static_cast<IFace*>( OGenericPluginLoader::load( item, uid ) );
}

}
}


#endif
