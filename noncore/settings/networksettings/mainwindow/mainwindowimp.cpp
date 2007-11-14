
#include "mainwindowimp.h"
#include "addconnectionimp.h"
#include "interfaceinformationimp.h"
#include "interfacesetupimp.h"
#include "interfaces.h"
#include "module.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oprocess.h>
#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/qlibrary.h>

/* QT */
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qheader.h>
#include <qlabel.h>
#include <qtabwidget.h> // in order to disable the profiles tab
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qregexp.h>


#if QT_VERSION < 0x030000
#include <qlist.h>
#else
#include <qptrlist.h>
#endif
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>

/* STD */
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#define DEFAULT_SCHEME "/var/lib/pcmcia/scheme"
#define _PROCNETDEV "/proc/net/dev"

// is this always right?
#define _HOSTFILE "/etc/hostname"
#define _IRDANAME "/proc/sys/net/irda/devname"

using namespace Opie::Core;

MainWindowImp::MainWindowImp(QWidget *parent, const char *name, WFlags) : MainWindow(parent, name, Qt::WStyle_ContextHelp), advancedUserMode(true), scheme(DEFAULT_SCHEME)
{
    connect(addConnectionButton, SIGNAL(clicked()), this, SLOT(addClicked()));
    connect(removeConnectionButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
    connect(informationConnectionButton, SIGNAL(clicked()), this, SLOT(informationClicked()));
    connect(configureConnectionButton, SIGNAL(clicked()), this, SLOT(configureClicked()));

    //remove tab with no function
    tabWidget->removePage( tab );

    // Load connections.
    // /usr/local/kde/lib/libinterfaces.la
    loadModules(QPEApplication::qpeDir() + "plugins/networksettings");
    getAllInterfaces();

    Interfaces i;
    QStringList list = i.getInterfaceList();
    QMap<QString, Interface*>::Iterator it;
    for ( QStringList::Iterator ni = list.begin(); ni != list.end(); ++ni )
    {
        /*
         * we skipped it in getAllInterfaces now
         * we need to ignore it as well
         */
        if (m_handledIfaces.contains( *ni) )
        {
            odebug << "Not up iface handled by module" << oendl;
            continue;
        }
        bool found = false;
        for( it = interfaceNames.begin(); it != interfaceNames.end(); ++it )
        {
            if(it.key() == (*ni))
                found = true;
        }
        if(!found)
        {
            if(!(*ni).contains("_"))
            {
                Interface *i = new Interface(this, *ni, false);
                i->setAttached(false);
                i->setHardwareName(tr("Disconnected"));
                interfaceNames.insert(i->getInterfaceName(), i);
                updateInterface(i);
                connect(i, SIGNAL(updateInterface(Interface*)), this, SLOT(updateInterface(Interface*)));
            }
        }
    }

    //getInterfaceList();
    connectionList->header()->hide();

    Config cfg("NetworkSetup");
    profiles = QStringList::split(" ", cfg.readEntry("Profiles", "All"));
    for ( QStringList::Iterator it = profiles.begin();
            it != profiles.end(); ++it)
        profilesList->insertItem((*it));
    currentProfileLabel->setText(cfg.readEntry("CurrentProfile", "All"));
    advancedUserMode = cfg.readBoolEntry("AdvancedUserMode", false);
    scheme = cfg.readEntry("SchemeFile", DEFAULT_SCHEME);

    QFile file(scheme);
    if ( file.open(IO_ReadOnly) )
    {    // file opened successfully
        QTextStream stream( &file );        // use a text stream
        while ( !stream.eof() )
        {        // until end of file...
            QString line = stream.readLine();       // line of text excluding '\n'
            if(line.contains("SCHEME"))
            {
                line = line.mid(7, line.length());
                currentProfileLabel->setText(line);
                break;
            }
        }
        file.close();
    }
    makeChannel();
    initHostname();
}

/**
 * Deconstructor. Save profiles.  Delete loaded libraries.
 */
MainWindowImp::~MainWindowImp()
{
    // Save profiles.
    Config cfg("NetworkSetup");
    cfg.setGroup("General");
    cfg.writeEntry("Profiles", profiles.join(" "));

    // Delete all interfaces that don't have owners.
    QMap<Interface*, QListViewItem*>::Iterator iIt;
    for( iIt = items.begin(); iIt != items.end(); ++iIt )
    {
        if(iIt.key()->getModuleOwner() == NULL)
            delete iIt.key();
    }

    // Delete Modules and Libraries
    QMap<Module*, QLibrary*>::Iterator it;
    for( it = libraries.begin(); it != libraries.end(); ++it )
    {
        delete it.key();
        // I wonder why I can't delete the libraries
        // What fucking shit this is.
        //delete it.data();
    }
}

/**
 * Query the kernel for all of the interfaces.
 */
void MainWindowImp::getAllInterfaces()
{
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
        return;

    struct ifreq ifr;
    QStringList ifaces;
    QFile procFile(QString(_PROCNETDEV));
    int result;
    Interface *i;

    if (! procFile.exists())
    {
        struct ifreq ifrs[100];
        struct ifconf ifc;
        ifc.ifc_len = sizeof(ifrs);
        ifc.ifc_req = ifrs;
        result = ioctl(sockfd, SIOCGIFCONF, &ifc);

        for (unsigned int i = 0; i < ifc.ifc_len / sizeof(struct ifreq); i++)
        {
            struct ifreq *pifr = &ifrs[i];
            if ( !QString( pifr->ifr_name ).startsWith( "wifi" ) ) ifaces += pifr->ifr_name;
            else odebug << "ignoring hostap control interface " << pifr->ifr_name << oendl;
        }
    }
    else
    {
        if (!procFile.open(IO_ReadOnly))
            owarn << "Failed to open proc file " << procFile.name() << oendl;
        QString line;
        QTextStream procTs(&procFile);
        int loc = -1;

        procTs.readLine(); // eat a line
        procTs.readLine(); // eat a line
        while((line = procTs.readLine().simplifyWhiteSpace()) != QString::null)
        {
            if((loc = line.find(":")) != -1)
            {
                // ignore wifi* (hostap control interfaces)
                if ( !line.left(loc).startsWith( "wifi" ) ) ifaces += line.left(loc);
                else odebug << "ignoring hostap control interface " << line.left(loc) << oendl;
            }
        }
    }

    for (QStringList::Iterator it = ifaces.begin(); it != ifaces.end(); ++it)
    {
        int flags = 0;
        if ( m_handledIfaces.contains( (*it) ) )
        {
            odebug << " " << (*it).latin1() << " is handled by a module" << oendl;
            continue;
        }
        //    int family;
        i = NULL;

        strncpy(ifr.ifr_name, (*it).latin1(), 16);

        struct ifreq ifcopy;
        ifcopy = ifr;
        result = ioctl(sockfd, SIOCGIFFLAGS, &ifcopy);
        flags = ifcopy.ifr_flags;
        i = new Interface(this, ifr.ifr_name, false);
        i->setAttached(true);
        if ((flags & IFF_UP) == IFF_UP)
            i->setStatus(true);
        else
            i->setStatus(false);

        if ((flags & IFF_BROADCAST) == IFF_BROADCAST)
            i->setHardwareName("Ethernet");
        else if ((flags & IFF_POINTOPOINT) == IFF_POINTOPOINT)
            i->setHardwareName("Point to Point");
        else if ((flags & IFF_MULTICAST) == IFF_MULTICAST)
            i->setHardwareName("Multicast");
        else if ((flags & IFF_LOOPBACK) == IFF_LOOPBACK)
            i->setHardwareName("Loopback");
        else
            i->setHardwareName("Unknown");

        owarn << "Adding interface " << ifr.ifr_name << " to interfaceNames\n" << oendl;
        interfaceNames.insert(i->getInterfaceName(), i);
        updateInterface(i);
        connect(i, SIGNAL(updateInterface(Interface*)),
                this, SLOT(updateInterface(Interface*)));
    }
    // now lets ask the plugins too ;)
    QMap<Module*, QLibrary*>::Iterator it;
    QList<Interface> ilist;
    for( it = libraries.begin(); it != libraries.end(); ++it )
    {
        if(it.key())
        {
            ilist = it.key()->getInterfaces();
            for( i = ilist.first(); i != 0; i = ilist.next() )
            {
                owarn << "Adding interface " << i->getInterfaceName().latin1() << " to interfaceNames\n" << oendl;
                interfaceNames.insert(i->getInterfaceName(), i);
                updateInterface(i);
                connect(i, SIGNAL(updateInterface(Interface*)),
                        this, SLOT(updateInterface(Interface*)));
            }
        }
    }
}

/**
 * Load all modules that are found in the path
 * @param path a directory that is scaned for any plugins that can be loaded
 *  and attempts to load them
 */
void MainWindowImp::loadModules(const QString &path)
{
#ifdef DEBUG
    odebug << "MainWindowImp::loadModules: " << path.latin1() << "" << oendl;
#endif
    QDir d(path);
    if(!d.exists())
        return;

    QString lang = ::getenv("LANG");
    // Don't want sym links
    d.setFilter( QDir::Files | QDir::NoSymLinks );
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;
    while ( (fi=it.current()) )
    {
        if(fi->fileName().contains(".so"))
        {
            /* if loaded install translation */
            if( loadPlugin(path + "/" + fi->fileName()) != 0l ){
                QTranslator *trans = new QTranslator(qApp);
                QString fn = QPEApplication::qpeDir()+"i18n/"+lang+"/"+ fi->fileName().left( fi->fileName().find(".") )+".qm";
                if( trans->load( fn ) )
                    qApp->installTranslator( trans );
                else
                    delete trans;
            }
            odebug << "loaded plugin: >" << QString(path + "/" + fi->fileName()).latin1() << "< " << oendl;
        }
        ++it;
    }
}

/**
 * Attempt to load a function and resolve a function.
 * @param pluginFileName - the name of the file in which to attempt to load
 * @param resolveString - function pointer to resolve
 * @return pointer to the function with name resolveString or NULL
 */
Module* MainWindowImp::loadPlugin(const QString &pluginFileName, const QString &resolveString)
{
#ifdef DEBUG
    odebug << "MainWindowImp::loadPlugin: " << pluginFileName.latin1() << ": resolving " << resolveString.latin1() << "" << oendl;
#endif
    QLibrary *lib = new QLibrary(pluginFileName);
    void *functionPointer = lib->resolve(resolveString);
    if( !functionPointer )
    {
#ifdef DEBUG
        odebug << "MainWindowImp::loadPlugin: Warning: " << pluginFileName.latin1() << " is not a plugin" << oendl;
#endif
        delete lib;
        return 0;
    }
    // Try to get an object.
    Module *object = ((Module* (*)()) functionPointer)();
    if(object == 0)
    {
#ifdef DEBUG
        odebug << "MainWindowImp: Couldn't create object, but did load library!" << oendl;
#endif
        delete lib;
        return 0;
    }

    m_handledIfaces += object->handledInterfaceNames();
    // Store for deletion later
    libraries.insert(object, lib);
    return object;
}

/**
 * The Add button was clicked.  Bring up the add dialog and if OK is hit
 * load the plugin and append it to the list
 */
void MainWindowImp::addClicked()
{
    QMap<Module*, QLibrary*>::Iterator it;
    QMap<QString, QString> list;
    QMap<QString, Module*> newInterfaceOwners;

    for( it = libraries.begin(); it != libraries.end(); ++it )
    {
        if(it.key())
        {
            (it.key())->possibleNewInterfaces(list);
        }
    }
    // See if the list has anything that we can add.
    if(list.count() == 0)
    {
        QMessageBox::information(this, "Sorry", "Nothing to add.", QMessageBox::Ok);
        return;
    }
    AddConnectionImp addNewConnection(this, "AddConnectionImp", true);
    addNewConnection.addConnections(list);
    if( QDialog::Accepted == QPEApplication::execDialog( &addNewConnection ) )
    {
        QListViewItem *item = addNewConnection.registeredServicesList->currentItem();
        if(!item)
            return;

        for( it = libraries.begin(); it != libraries.end(); ++it )
        {
            if(it.key())
            {
                Interface *i = (it.key())->addNewInterface(item->text(0));
                if(i)
                {
                    odebug << "iface name " << i->getInterfaceName().latin1() << "" << oendl;
                    interfaceNames.insert(i->getInterfaceName(), i);
                    updateInterface(i);
                }
            }
        }
    }
}

/**
 * Prompt the user to see if they really want to do this.
 * If they do then remove from the list and unload.
 */
void MainWindowImp::removeClicked()
{
    QListViewItem *item = connectionList->currentItem();
    if(!item)
    {
        QMessageBox::information(this, "Sorry","Please select an interface First.", QMessageBox::Ok);
        return;
    }

    Interface *i = interfaceItems[item];
    if(i->getModuleOwner() == NULL)
    {
        QMessageBox::information(this, "Can't remove interface.", "Interface is built in.", QMessageBox::Ok);
    }
    else
    {
        if(!i->getModuleOwner()->remove(i))
            QMessageBox::information(this, tr("Error"), tr("Unable to remove."), QMessageBox::Ok);
        else
        {
            delete item;
            //      QMessageBox::information(this, "Success", "Interface was removed.", QMessageBox::Ok);
        }
    }
}

/**
 * Pull up the configure about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure.
 */
void MainWindowImp::configureClicked()
{
    QListViewItem *item = connectionList->currentItem();
    if(!item)
    {
        QMessageBox::information(this, tr("Sorry"),tr("Please select an interface first."), QMessageBox::Ok);
        return;
    }

    QString currentProfileText = currentProfileLabel->text();
    if(currentProfileText.upper() == "ALL");
    currentProfileText = "";

    Interface *i = interfaceItems[item];

    if(i->getModuleOwner())
    {
        QWidget *moduleConfigure = i->getModuleOwner()->configure(i);
        if(moduleConfigure != NULL)
        {
            i->getModuleOwner()->setProfile(currentProfileText);
            QPEApplication::showWidget( moduleConfigure );
            return;
        }
    }

    InterfaceSetupImpDialog *configure = new InterfaceSetupImpDialog(this, "InterfaceSetupImp", i, true, Qt::WDestructiveClose  | Qt::WStyle_ContextHelp );
    configure->setProfile(currentProfileText);
    QPEApplication::showDialog( configure );
}

/**
 * Pull up the information about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure.
 */
void MainWindowImp::informationClicked()
{
    QListViewItem *item = connectionList->currentItem();
    if(!item)
    {
        QMessageBox::information(this, "Sorry","Please select an interface First.", QMessageBox::Ok);
        return;
    }

    Interface *i = interfaceItems[item];
    //   if(!i->isAttached()){
    //     QMessageBox::information(this, "Sorry","No information about\na disconnected interface.", QMessageBox::Ok);
    //     return;
    //   }

    if(i->getModuleOwner())
    {
        QWidget *moduleInformation = i->getModuleOwner()->information(i);
        if(moduleInformation != NULL)
        {
            QPEApplication::showWidget( moduleInformation );
#ifdef DEBUG
            odebug << "MainWindowImp::informationClicked:: Module owner has created, we showed." << oendl;
#endif
            return;
        }
    }
    InterfaceInformationImp *information = new InterfaceInformationImp(this, "InterfaceSetupImp", i, Qt::WType_Modal | Qt::WDestructiveClose | Qt::WStyle_Dialog  | Qt::WStyle_ContextHelp);
    QPEApplication::showWidget( information );
}

/**
 * Update this interface.  If no QListViewItem exists create one.
 * @param Interface* pointer to the interface that needs to be updated.
 */
void MainWindowImp::updateInterface(Interface *i)
{
    if(!advancedUserMode)
    {
        if(i->getInterfaceName() == "lo")
            return;
    }

    QListViewItem *item = NULL;

    // Find the interface, making it if needed.
    if(items.find(i) == items.end())
    {
        item = new QListViewItem(connectionList, "", "", "");
        // See if you can't find a module owner for this interface
        QMap<Module*, QLibrary*>::Iterator it;
        for( it = libraries.begin(); it != libraries.end(); ++it )
        {
            if(it.key()->isOwner(i))
                i->setModuleOwner(it.key());
        }
        items.insert(i, item);
        interfaceItems.insert(item, i);
    }
    else
        item = items[i];

    // Update the icons and information
    item->setPixmap( 0, Opie::Core::OResource::loadPixmap( i->getStatus() ? "up" : "down", Opie::Core::OResource::SmallIcon ) );

    QString typeName = "lan";
    if(i->getInterfaceName() == "lo")
        typeName = "lo";
    if(i->getInterfaceName().contains("irda"))
        typeName = "irda";
    if(i->getInterfaceName().contains("wlan"))
        typeName = "wlan";
    if(i->getInterfaceName().contains("usb"))
        typeName = "usb";

    if(!i->isAttached())
        typeName = "connect_no";
    // Actually try to use the Module
    if(i->getModuleOwner() != NULL)
        typeName = i->getModuleOwner()->getPixmapName(i);

    item->setPixmap( 1, ( Opie::Core::OResource::loadPixmap( "networksettings/" + typeName, Opie::Core::OResource::SmallIcon ) ) );
    
    item->setText(2, i->getHardwareName());
    item->setText(3, QString("(%1)").arg(i->getInterfaceName()));
    item->setText(4, (i->getStatus()) ? i->getIp() : QString(""));
}

void MainWindowImp::newProfileChanged(const QString& newText)
{
    if(newText.length() > 0)
        newProfileButton->setEnabled(true);
    else
        newProfileButton->setEnabled(false);
}

/**
 * Adds a new profile to the list of profiles.
 * Don't add profiles that already exists.
 * Appends to the list and QStringList
 */
void MainWindowImp::addProfile()
{
    QString newProfileName = newProfile->text();
    if(profiles.grep(newProfileName).count() > 0)
    {
        QMessageBox::information(this, "Can't Add","Profile already exists.", QMessageBox::Ok);
        return;
    }
    profiles.append(newProfileName);
    profilesList->insertItem(newProfileName);
}

/**
 * Removes the currently selected profile in the combo.
 * Doesn't delete if there are less then 2 profiles.
 */
void MainWindowImp::removeProfile()
{
    if(profilesList->count() <= 1)
    {
        QMessageBox::information(this, "Can't remove.","At least one profile\nis needed.", QMessageBox::Ok);
        return;
    }
    QString profileToRemove = profilesList->currentText();
    if(profileToRemove == "All")
    {
        QMessageBox::information(this, "Can't remove.","Can't remove default.", QMessageBox::Ok);
        return;
    }
    // Can't remove the curent profile
    if(profileToRemove == currentProfileLabel->text())
    {
        QMessageBox::information(this, "Can't remove.",QString("%1 is the current profile.").arg(profileToRemove), QMessageBox::Ok);
        return;

    }

    if(QMessageBox::information(this, "Question",QString("Remove profile: %1").arg(profileToRemove), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
    {
        profiles = QStringList::split(" ", profiles.join(" ").replace(QRegExp(profileToRemove), ""));
        profilesList->clear();
        for ( QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it)
            profilesList->insertItem((*it));

        // Remove any interface settings and mappings.
        Interfaces interfaces;
        // Go through them one by one
        QMap<Interface*, QListViewItem*>::Iterator it;
        for( it = items.begin(); it != items.end(); ++it )
        {
            QString interfaceName = it.key()->getInterfaceName();
            odebug << interfaceName.latin1() << oendl;
            if(interfaces.setInterface(interfaceName + "_" + profileToRemove))
            {
                interfaces.removeInterface();
                if(interfaces.setMapping(interfaceName))
                {
                    if(profilesList->count() == 1)
                        interfaces.removeMapping();
                    else
                    {
                        interfaces.removeMap("map", interfaceName + "_" + profileToRemove);
                    }
                }
                interfaces.write();
                break;
            }
        }
    }
}

/**
 * A new profile has been selected, change.
 * @param newProfile the new profile.
 */
void MainWindowImp::changeProfile()
{
    if(profilesList->currentItem() == -1)
    {
        QMessageBox::information(this, "Can't Change.","Please select a profile.", QMessageBox::Ok);
        return;
    }
    QString newProfile = profilesList->text(profilesList->currentItem());
    if(newProfile != currentProfileLabel->text())
    {
        currentProfileLabel->setText(newProfile);
        QFile::remove(scheme);
        QFile file(scheme);
        if ( file.open(IO_ReadWrite) )
        {
            QTextStream stream( &file );
            stream << QString("SCHEME=%1").arg(newProfile);
            file.close();
        }
        // restart all up devices?
        if(QMessageBox::information(this, "Question","Restart all running interfaces?", QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok)
        {
            // Go through them one by one
            QMap<Interface*, QListViewItem*>::Iterator it;
            for( it = items.begin(); it != items.end(); ++it )
            {
                if(it.key()->getStatus() == true)
                    it.key()->restart();
            }
        }
    }
    // TODO change the profile in the modules
}


void MainWindowImp::makeChannel()
{
    channel = new QCopChannel( "QPE/Application/networksettings", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(receive(const QCString&,const QByteArray&)) );
}

void MainWindowImp::receive(const QCString &msg, const QByteArray &arg)
{
    bool found = false;
    odebug << "MainWindowImp::receive QCop msg >"+msg+"<" << oendl;
    if (msg == "raise")
    {
        raise();
        return;
    }

    QString dest = msg.left(msg.find("("));
    QCString param = msg.right(msg.length() - msg.find("(") - 1);
    param = param.left( param.length() - 1 );
    odebug << "dest >" << dest.latin1() << "< param >"+param+"<" << oendl;

    QMap<Module*, QLibrary*>::Iterator it;
    for( it = libraries.begin(); it != libraries.end(); ++it )
    {
        odebug << "plugin >" << it.key()->type().latin1() << "<" << oendl;
        if(it.key()->type() == dest)
        {
            it.key()->receive( param, arg );
            found = true;
        }
    }


    if (found) QPEApplication::setKeepRunning();
    else odebug << "Huh what do ya want" << oendl;
}

void MainWindowImp::setHostname()
{
    static QRegExp filter("[^A-Za-z0-9_\\-\\.]");
    if (filter.match(m_Nameinput->text())!=-1) {
        odebug << "Wrong hostname" << oendl;
        QMessageBox::critical(0, tr("Sorry"), tr("This is a wrong name.<br>Please use A-Z, a-z, _, - or a single dot."));
        return;
    }

    OProcess h;
    _procTemp="";
    h << "hostname" << m_Nameinput->text();
    connect(&h,SIGNAL(receivedStderr(Opie::Core::OProcess*,char*,int)),this,SLOT(slotHostname(Opie::Core::OProcess*,char*,int)));
    if (!h.start(OProcess::Block,OProcess::Stderr))
        owarn << "Failed execution of 'hostname'. Are the paths correct?" << oendl;
    odebug << "Got " << _procTemp << " - " << h.exitStatus() << oendl;
    if (h.exitStatus()!=0) {
        QMessageBox::critical(0, tr("Sorry"), QString(tr("Could not set name.\n%1")).arg(_procTemp.stripWhiteSpace()));
        return;
    }
    _procTemp="";

    QFile f(_HOSTFILE);
    if (f.open(IO_Truncate|IO_WriteOnly))
    {
        QTextStream s(&f);
        s << m_Nameinput->text();
    } else {
        QMessageBox::critical(0, tr("Sorry"), tr("Could not save name."));
        return;
    }

    f.close();
    f.setName(_IRDANAME);
    if (f.open(IO_WriteOnly))
    {
        QTextStream s(&f);
        s << m_Nameinput->text();
    } else {
        QMessageBox::critical(0, tr("Sorry"), tr("Could not set infrared name."));
    }
}

void MainWindowImp::initHostname()
{
    OProcess h;
    _procTemp="";

    h << "hostname";
    connect(&h,SIGNAL(receivedStdout(Opie::Core::OProcess*,char*,int)),this,SLOT(slotHostname(Opie::Core::OProcess*,char*,int)));
    if (!h.start(OProcess::Block,OProcess::AllOutput))
        owarn << "Could not execute 'hostname'. Are the paths correct?" oendl;
    odebug << "Got " << _procTemp <<oendl;
    m_Nameinput->setText(_procTemp.stripWhiteSpace());
    _procTemp="";
}

void MainWindowImp::slotHostname(Opie::Core::OProcess * /*proc*/, char *buffer, int buflen)
{
    if (buflen < 1 || buffer==0) return;
    char*_t = new char[buflen+1];
    ::memset(_t,0,buflen+1);
    ::memcpy(_t,buffer,buflen);
    _procTemp+=_t;
    delete[]_t;
}
