
#include "modem.h"
#include "pppconfig.h"
#include "pppmodule.h"
#include "pppdata.h"
#include "interfaceinformationppp.h"
#include "interfaceppp.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
using namespace Opie::Core;

/* QT */

/* STD */
#include <errno.h>
#include <signal.h>

// don't polute global namespace
namespace
{
    /*
     * If network settings is qutting and we've ppp
     * devices open we need to save the pid_t the PPData
     * and the interface number
     */
    struct Connection
    {
        pid_t pid;
        QString device;
        QString name;
    };
    class InterfaceKeeper
    {
    public:
        InterfaceKeeper();
        ~InterfaceKeeper();

        void addInterface( pid_t, const QString& pppDev, const QString& name );
        QMap<QString, Connection> interfaces()const; // will check if still available
    private:
        bool isAvailable( pid_t )const;
        QMap<QString, Connection> m_interfaces;
    };
}


/**
 * Constructor, find all of the possible interfaces
 * We also need to restore the state.. it could be that
 * an interface was up while closing the application
 * we need to be able to shut it down...
 */
PPPModule::PPPModule() : Module()
{
    InterfaceKeeper inFace;
    QMap<QString,Connection> running = inFace.interfaces();
    QStringList handledInterfaceNames;

    QMap<QString,QString> ifaces = PPPData::getConfiguredInterfaces();
    QMap<QString,QString>::Iterator it;
    InterfacePPP *iface;
    odebug << "getting interfaces" << oendl; 
    for( it = ifaces.begin(); it != ifaces.end(); ++it )
    {
        odebug << "ifaces " << it.key().latin1() << " " << it.data().latin1() << "" << oendl; 
        iface = new InterfacePPP( 0, it.key() );
        iface->setHardwareName( it.data() );
        list.append( (Interface*)iface );

        // check if (*it) is one of the running ifaces
        if ( running.contains( it.data() ) )
        {
            odebug << "iface is running " << it.key().latin1() << "" << oendl; 
            handledInterfaceNames << running[it.data()].device;
            iface->setStatus( true );
            iface->setPPPDpid( running[it.data()].pid );
            iface->modem()->setPPPDevice( running[it.data()].device );
            iface->refresh();
        }
    }

    setHandledInterfaceNames( handledInterfaceNames );
}

/**
 * Delete any interfaces that we own.
 */
PPPModule::~PPPModule()
{
    odebug << "PPPModule::~PPPModule() " << oendl; 
    QMap<QString,QString> ifaces;
    InterfaceKeeper keeper;
    Interface *i;
    for ( i=list.first(); i != 0; i=list.next() )
    {
        /* if online save the state */
        if ( i->getStatus() )
        {
            odebug << "Iface " << i->getHardwareName().latin1() << " is still up" << oendl; 
            InterfacePPP* ppp = static_cast<InterfacePPP*>(i);
            keeper.addInterface( ppp->pppPID(), ppp->pppDev(), ppp->getHardwareName() );
        }
        ifaces.insert( i->getInterfaceName(), i->getHardwareName() );
        delete i;
    }
    PPPData::setConfiguredInterfaces( ifaces );
}

/**
 * Change the current profile
 */
void PPPModule::setProfile(const QString &newProfile)
{
    profile = newProfile;
}

/**
 * get the icon name for this device.
 * @param Interface* can be used in determining the icon.
 * @return QString the icon name (minus .png, .gif etc)
 */
QString PPPModule::getPixmapName(Interface* )
{
    return "ppp";
}

/**
 * Check to see if the interface i is owned by this module.
 * @param Interface* interface to check against
 * @return bool true if i is owned by this module, false otherwise.
 */
bool PPPModule::isOwner(Interface *i)
{
    return list.find( i ) != -1;
}

/**
 * Create, and return the WLANConfigure Module
 * @return QWidget* pointer to this modules configure.
 */
QWidget *PPPModule::configure(Interface *i)
{
    odebug << "return ModemWidget" << oendl; 
    PPPConfigWidget *pppconfig = new PPPConfigWidget( (InterfacePPP*)i,
                                 0, "PPPConfig", false,
                                 (Qt::WDestructiveClose | Qt::WStyle_ContextHelp));
    return pppconfig;
}

/**
 * Create, and return the Information Module
 * @return QWidget* pointer to this modules info.
 */
QWidget *PPPModule::information(Interface *i)
{
    // We don't have any advanced pppd information widget yet :-D
    // TODO ^

    return new InterfaceInformationPPP( 0, "InterfaceInformationPPP", i );
}

/**
 * Get all active (up or down) interfaces
 * @return QList<Interface> A list of interfaces that exsist that havn't
 * been called by isOwner()
 */
QList<Interface> PPPModule::getInterfaces()
{
    // List all of the files in the peer directory
    odebug << "PPPModule::getInterfaces" << oendl; 
    return list;
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */
Interface *PPPModule::addNewInterface(const QString &newInterface)
{

    InterfacePPP *ifaceppp;
    Interface *iface;
    ifaceppp = new InterfacePPP();
    PPPConfigWidget imp(ifaceppp, 0, "PPPConfigImp", true);

    if( QPEApplication::execDialog( &imp ) == QDialog::Accepted )
    {
        iface = (InterfacePPP*) ifaceppp;
        iface->setModuleOwner( this );
        list.append( iface );
        return iface;
    }
    else
    {
        delete ifaceppp;
        iface = NULL;
    }
    return iface;
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successful, false otherwise.
 */
bool PPPModule::remove(Interface *i)
{
    return list.remove(i);
}

void PPPModule::possibleNewInterfaces(QMap<QString, QString> &newIfaces)
{
    newIfaces.insert(QObject::tr("PPP") ,
                     QObject::tr("generic ppp device"));
}



namespace
{
    InterfaceKeeper::InterfaceKeeper( )
    {}
    InterfaceKeeper::~InterfaceKeeper()
    {
        Config cfg("ppp_plugin_keeper");
        QStringList lst = cfg.groupList();
        for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
        {
            Connection con;
            cfg.setGroup( (*it) );
            cfg.clearGroup();
        }

        for (QMap<QString, Connection>::Iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it )
        {
            Connection con = it.data();
            cfg.setGroup( con.name );
            cfg.writeEntry( "pid", con.pid );
            cfg.writeEntry( "device", con.device );
        }
    }
    void InterfaceKeeper::addInterface(pid_t pid, const QString& dev, const QString& name )
    {
        Connection con;
        con.pid = pid;
        con.device = dev;
        con.name = name;
        m_interfaces.insert( name, con );
    }
    QMap<QString, Connection> InterfaceKeeper::interfaces()const
    {
        Config cfg("ppp_plugin_keeper");
        QMap<QString, Connection> ifaces;
        QStringList lst = cfg.groupList();
        for (QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
        {
            Connection con;
            cfg.setGroup( (*it) );
            con.name = (*it);
            con.pid = cfg.readNumEntry("pid");
            con.device = cfg.readEntry("device");
            odebug << " " << con.name.latin1() << " " << con.device.latin1() << " " << con.pid << "" << oendl; 

            if ( con.pid != -1 && isAvailable( con.pid ) )
                ifaces.insert( con.name, con );
        }
        return ifaces;
    }
    bool InterfaceKeeper::isAvailable( pid_t p)const
    {
        if (::kill(p, 0 ) == 0 || errno != ESRCH )
        {
            odebug << "isAvailable " << p << "" << oendl; 
            return true;
        }

        odebug << "notAvailable " << p << "" << oendl; 
        return false;
    }

}
