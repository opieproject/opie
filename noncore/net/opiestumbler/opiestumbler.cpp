#include <cstdlib>

#include <qmenubar.h>
#include <qaction.h>
#include <qlistview.h>
#include <qstring.h>
//#include <qapplication.h>
#include <qdatetime.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qtimer.h>
#include <qprogressbar.h>

#include <qpe/config.h>
//#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include <opie2/odebug.h>
#include <opie2/ostation.h>
#include <opie2/omanufacturerdb.h>
#include <opie2/onetwork.h>
#include <opie2/oprocess.h>

#include "stumbler.h"
#include "opiestumbler.h"
#include "stumblersettings.h"
#include "stationviewitem.h"
#include "stumblerstation.h"
#include "stationinfo.h"
    

using Opie::Net::OWirelessNetworkInterface;
using Opie::Net::ONetwork;


QString OpieStumbler::appCaption() {
    return QObject::tr("OpieStumbler");
}

OpieStumbler::OpieStumbler(QWidget *parent, const char *name, WFlags)
    :QMainWindow(parent, name, WStyle_ContextHelp),
    m_listCurrent(new QListView(this)), m_listHistory(new QListView(this)),
    m_stationsCurrent(new QList<Opie::Net::OStation>),
    m_popupCurrent(new QPopupMenu(this)),
    m_popupHistory(new QPopupMenu(this)),
    m_db(NULL), m_proc(NULL)
{
    
    if ( QCopChannel::isRegistered("QPE/OpieStumbler") ) {
         QCopEnvelope e("QPE/OpieStumbler", "show()");
         exit(EXIT_SUCCESS);
    }

    QGridLayout *grid = new QGridLayout( this, 1, 1, 3, 0, "grid");
    QVBoxLayout *lay = new QVBoxLayout( NULL, 0, 5, "lay" );
    QSpacerItem *spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed );
    lay->addItem(spacer);
    lay->addWidget(m_listCurrent);
    lay->addWidget(m_listHistory);
    grid->addLayout(lay, 0, 0);

    m_stationsCurrent->setAutoDelete(true);
    
    m_channel = new QCopChannel( "QPE/OpieStumbler", this );
    connect(m_channel, SIGNAL(received(const QCString &, const QByteArray &)), 
            this, SLOT(slotMessageReceived( const QCString &, const QByteArray &)) );
    
    //setCaption(appCaption());
    //setCentralWidget(grid);
    setToolBarsMovable(false);
    
    
    QPopupMenu *fileMenu = new QPopupMenu(this);
    QPopupMenu *configMenu = new QPopupMenu(this);
    QPopupMenu *scanMenu = new QPopupMenu(this);
    
    fileMenu->insertItem( tr("Exit"), this, SLOT(close()) );
    configMenu->insertItem( tr("Configure"), this, SLOT(slotConfigure()) );
    scanMenu->insertItem( tr("Start"), this, SLOT(slotStartScanning()) );
    scanMenu->insertItem( tr("Stop"), this, SLOT(slotStopScanning()) );

    m_popupCurrent->insertItem( tr("Show details"), this, SLOT(slotShowDetails()) );
    m_popupCurrent->insertItem( tr("Join Network"), this, SLOT(slotJoinNetwork()) );
    
    menuBar()->insertItem(tr("File"), fileMenu);
    menuBar()->insertItem(tr("Settings"), configMenu);
    menuBar()->insertItem(tr("Scanning"), scanMenu);
    
    QPEApplication::setStylusOperation(m_listCurrent->viewport(), QPEApplication::RightOnHold);
    QPEApplication::setStylusOperation(m_listHistory->viewport(), QPEApplication::RightOnHold);
    
    m_listCurrent->addColumn(tr("SSID"));
    m_listCurrent->addColumn(tr("Chan"));
    m_listCurrent->addColumn(tr("Signal"));
    m_listCurrent->addColumn(tr("Enc"));
    m_listCurrent->setSelectionMode( QListView::Extended );

    m_listHistory->addColumn(tr("SSID"));
    m_listHistory->addColumn(tr("Chan"));
    m_listHistory->addColumn(tr("Max Sig"));
    m_listHistory->addColumn(tr("Enc"));
    m_listHistory->addColumn(tr("Vendor"));
   
    connect(m_listCurrent, SIGNAL(mouseButtonPressed (int, QListViewItem*, const QPoint&, int)),
            this, SLOT(slotCurrentMousePressed (int, QListViewItem*, const QPoint&, int)));

    connect(m_listHistory, SIGNAL(mouseButtonPressed (int, QListViewItem*, const QPoint&, int)),
            this, SLOT(slotHistoryMousePressed (int, QListViewItem*, const QPoint&, int)));
    
    for(int i = CURCHAN; i <= CURENC; ++i) {
        m_listCurrent->setColumnAlignment( i, Qt::AlignHCenter );
        m_listHistory->setColumnAlignment( i, Qt::AlignHCenter );
    }

    loadConfig();
    m_stumbler = new Stumbler(m_interface, this);
    connect(m_stumbler, SIGNAL(newdata()), this, SLOT(slotUpdateStations()));

    QTimer::singleShot(1000, this, SLOT(slotLoadManufacturers()) );
    
    slotStartScanning();
}

void OpieStumbler::slotConfigure()
{
    StumblerSettings settings(this, "Settings", true);
    if (settings.exec() == QDialog::Accepted)
    loadConfig();
}

void OpieStumbler::loadConfig()
{
    Config cfg("OpieStumbler", Config::User);
    cfg.setGroup("General");
    m_interface = cfg.readEntry("interface", "wlan0");
}

void OpieStumbler::slotStartScanning()
{
    setCaption(appCaption() + " (" + tr("Scanning") + ")");
    m_stumbler->start();
}

void OpieStumbler::slotStopScanning()
{
    setCaption(appCaption());
    m_stumbler->stop();
}

void OpieStumbler::slotUpdateStations()
{
    m_stationsCurrent->clear();
    
    m_stationsCurrent = m_stumbler->stations();
    if (m_stationsCurrent) {
        QListIterator<Opie::Net::OStation> it(*m_stationsCurrent);
        for(; it.current(); ++it) {
            Opie::Net::OStation *station = it.current();
            QListIterator<StumblerStation> itr(m_stationsHistory);
            for( ; itr.current(); ++itr) {
                if (itr.current()->st->macAddress.toString() == station->macAddress.toString()) {
                    break;
                }
            }
            if (!itr.current()) {
                //We need to copy the date because m_statiosCurrent has autodelete enabled
                m_stationsHistory.append(new StumblerStation(new Opie::Net::OStation, QDateTime::currentDateTime()));
                *(m_stationsHistory.last()->st) = (*station);
            }
            else {
                if ( itr.current()->st->level < station->level )
                    itr.current()->st->level = station->level;
                
                itr.current()->lastTimeSeen = QDateTime::currentDateTime();
            }   
        }
    }
    displayStations();
}

void OpieStumbler::displayStations()
{
    m_listCurrent->clear();
    for(QListIterator<Opie::Net::OStation> it(*m_stationsCurrent); it.current(); ++it)
        new StationViewItem( m_listCurrent, it.current()->ssid, QString::number(it.current()->channel),
                QString::number(it.current()->level), it.current()->encrypted ? "Y": "N", it.current()->macAddress.toString() );

    m_listHistory->clear();
    for(QListIterator<StumblerStation> it(m_stationsHistory); it.current(); ++it) 
        new StationViewItem( m_listHistory, it.current()->st->ssid, QString::number(it.current()->st->channel),
                QString::number(it.current()->st->level), it.current()->st->encrypted ? "Y": "N",
                manufacturer(it.current()->st->macAddress.toString()), it.current()->st->macAddress.toString() );
}

void OpieStumbler::slotMessageReceived( const QCString &message, const QByteArray &parameters)
{
    Q_UNUSED(const_cast<QByteArray &>(parameters))

    if ( message == "show()" )
        show();
}

void OpieStumbler::slotCurrentMousePressed(int button, QListViewItem * item, const QPoint &point, int c)
{
    Q_UNUSED(c)
        
    if ( 2 == button ) {
        m_mac = item->text(CURENC + 1);
        m_popupCurrent->popup(point);
    }
}


void OpieStumbler::slotHistoryMousePressed(int button, QListViewItem * item, const QPoint &point, int c)
{
    Q_UNUSED(c)
    
    if ( 2 == button ) {
        m_mac = item->text(HISVENDOR + 1);
        m_popupHistory->popup(point);
    }
}

void OpieStumbler::slotShowDetails()
{
    QListIterator<StumblerStation> it(m_stationsHistory);
    for(; it.current() && it.current()->st->macAddress.toString() != m_mac; ++it );
    
    if( it.current() ) {
        StationInfo info(it.current()->st->ssid, it.current()->st->type, QString::number(it.current()->st->channel),
                QString::number(it.current()->st->rates.last()/1000000), QString::number(it.current()->st->level),
                it.current()->st->encrypted ? "WEP": "No", 
                it.current()->st->macAddress.toString(), manufacturer(it.current()->st->macAddress.toString(), true),
                    it.current()->lastTimeSeen.toString() ,this, "", true);
        info.exec();
    }
    
} 

void OpieStumbler::slotLoadManufacturers()
{
    m_db = Opie::Net::OManufacturerDB::instance();
}

QString OpieStumbler::manufacturer( const QString &mac, bool extended )
{
    QString retval;
    if ( m_db )
        if ( extended )
            retval = m_db->lookupExt(mac);
        else
            retval = m_db->lookup(mac);

    if ( retval.isEmpty() )
        retval = tr("Unknown");

    return retval;
}

void OpieStumbler::slotJoinNetwork()
{
    slotStopScanning();

    OWirelessNetworkInterface *wiface = static_cast<OWirelessNetworkInterface*>(ONetwork::instance()->interface(m_interface)); 
    
    if( !wiface )
        return;
    
    QListIterator<StumblerStation> it(m_stationsHistory);
    
    for(; it.current() && it.current()->st->macAddress.toString() != m_mac; ++it );

    if( !it.current() )
        return;
    
    m_ssid = it.current()->st->ssid.left(it.current()->st->ssid.length()-1);
    m_splash = new QFrame( this, "splash", false, WStyle_StaysOnTop | WStyle_DialogBorder | WStyle_Customize );
    m_splash->setFrameStyle( QFrame::Panel | QFrame::Raised );
    m_splashBox = new QVBoxLayout( m_splash, 4, 4 );
    m_infoLabel = new QLabel( QString("<center><b>%1 %2</b></center>").arg(tr("Joining Network")).arg(m_ssid), m_splash );
    m_pbar = new QProgressBar( 3, m_splash );
    m_pbar->setCenterIndicator(true);
    m_splashBox->addWidget( m_infoLabel );
    m_splashBox->addWidget( m_pbar );
    int sw = m_splashBox->sizeHint().width() * 2;
    int sh = m_splashBox->sizeHint().height();
    m_splash->setGeometry((qApp->desktop()->width() - sw)/2, (qApp->desktop()->height() - sh)/2, sw, sh);
    m_splash->show();
    m_splash->raise();

    Opie::Net::OStation *station = it.current()->st;
    
    odebug << "Bringing interface down" << oendl;
    wiface->setUp(false);
    
    odebug << "Setting mode to " << (station->type == "adhoc" ? "adhoc" : "managed") << oendl;
    wiface->setMode(station->type == "adhoc" ? "adhoc" : "managed" );
    
    odebug << "Setting channel to " << station->channel << oendl;
    wiface->setChannel(station->channel);
    
    odebug << "Setting SSID to " << station->ssid << oendl;
    wiface->setSSID(station->ssid);
    
    wiface->commit();
    
    odebug << "Bringing interface up" << oendl;
    wiface->setUp(true);
    m_pbar->setProgress(1);
    //Wait 5 sec for association
    QTimer::singleShot(5000, this, SLOT(slotAssociated()));
}

void OpieStumbler::slotAssociated()
{
    OWirelessNetworkInterface *wiface = static_cast<OWirelessNetworkInterface*>(ONetwork::instance()->interface(m_interface));

    if( !wiface ) {
        slotCleanSplash();
        return;
    }

    if (!wiface->isAssociated()) {
        Global::statusMessage(tr("Could not Join"));
        m_infoLabel->setText(tr("Could not Join"));
        QTimer::singleShot(5000, this, SLOT(slotCleanSplash()));
        return;
    }
    
    Global::statusMessage(tr("Joined"));
    m_pbar->setProgress(2);
    m_infoLabel->setText(QString("<center><b>%1 %2</b></center>").arg(tr("Joined Network")).arg(m_ssid));
   
    if(m_proc) {
        m_proc->kill();
        delete m_proc;
    }

    m_proc = new Opie::Core::OProcess(this);
        
    *m_proc << "udhcpc" << "-f" << "-n" << "-i" << m_interface;
    m_proc->start(Opie::Core::OProcess::DontCare);
    QTimer::singleShot(5000, this, SLOT(slotCheckDHCP()));
}

void OpieStumbler::slotCheckDHCP()
{
    if(!m_proc->isRunning()) {
        Global::statusMessage(tr("Could not Obtain an Address"));
        m_infoLabel->setText(QString("<center><b>%1</b></center>").arg(tr("Could not Obtain an Address")));
        delete m_proc;
        m_proc = NULL;
        QTimer::singleShot(5000, this, SLOT(slotCleanSplash()));
        return;
    }
    m_listCurrent->show();
    m_pbar->setProgress(3);

    OWirelessNetworkInterface *wiface = static_cast<OWirelessNetworkInterface*>(ONetwork::instance()->interface(m_interface));
    QString ipv4 = wiface->ipV4Address().toString();
    m_infoLabel->setText(QString("<center><b>%1 %2</b></center>").arg(tr("Obtained IP")).arg(ipv4));
    Global::statusMessage(tr("Obtained IP") + " " + ipv4);
    QTimer::singleShot(5000, this, SLOT(slotCleanSplash()));
    
}

void OpieStumbler::slotCleanSplash()
{
    delete m_pbar;
    m_pbar = 0;  
    
    delete m_infoLabel;
    m_infoLabel = 0;
    
    delete m_splashBox;
    m_splashBox = 0;
    
    delete m_splash;
    m_splash = 0;           
}

