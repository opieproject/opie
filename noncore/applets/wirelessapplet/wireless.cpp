/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer <mickey@vanille.de>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "wireless.h"
#include "mgraph.h"
#include "advancedconfig.h"
#include "connect0.xpm"
#include "connect1.xpm"
#include "connect2.xpm"
#include "connect3.xpm"
#include "connect4.xpm"
#include "connect5.xpm"
#include "nowireless.xpm"

/* OPIE */
#include <opie2/onetwork.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/config.h>

/* QT */
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>

/* STD */
#include <sys/types.h>
#include <signal.h>

#define STYLE_BARS 0
#define STYLE_ANTENNA 1

//#define MDEBUG
#undef MDEBUG

WirelessControl::WirelessControl( WirelessApplet *applet, QWidget *parent, const char *name )
        : QFrame( parent, name, WStyle_StaysOnTop | WType_Popup ), applet( applet )
{

    readConfig();
    writeConfigEntry( "UpdateFrequency", updateFrequency );
    writeConfigEntry( "DisplayStyle", displayStyle );

    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    QGridLayout *grid = new QGridLayout( this, 3, 2, 6, 2, "top layout" );

    /* status label */

    statusLabel = new QLabel( this, "statuslabel" );
    QString text( "Wireless Status:<br>"
                  "*** Unknown ***<br>"
                  "Card not inserted ?<br>"
                  "Or Sharp ROM ?<br>"
                  "CELL: 00:00:00:00:00:00" );
    /*    QString text( "Station: Unknown<br>"
    	                "ESSID: Unknown<br>"
                        "MODE: Unknown<br>"
                        "FREQ: Unknown<br>"
                        "CELL: AA:BB:CC:DD:EE:FF" ); */
    statusLabel->setText( text );
    statusLabel->setFixedSize( statusLabel->sizeHint() );
    grid->addWidget( statusLabel, 0, 0 );

    /* visualization group box */

    QButtonGroup* group = new QButtonGroup( 1, Qt::Horizontal, "Visualization", this );
    QRadioButton* r1 = new QRadioButton( "Color Bars", group );
    QRadioButton* r2 = new QRadioButton( "Antenna", group );
    r1->setFocusPolicy( QWidget::NoFocus );
    r2->setFocusPolicy( QWidget::NoFocus );
    group->setFocusPolicy( QWidget::NoFocus );
    group->setButton( displayStyle );
    grid->addWidget( group, 0, 1 );

    /* quality graph */

    mgraph = new MGraph( this );
    mgraph->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    mgraph->setMin( 0 );
    mgraph->setMax( 92 );
    grid->addWidget( mgraph, 1, 0 );
    mgraph->setFocusPolicy( QWidget::NoFocus );

    /* advanced configuration Button */

    QPushButton* advanced = new QPushButton( "Advanced...", this );
    advanced->setFocusPolicy( QWidget::NoFocus );
    grid->addWidget( advanced, 2, 0, Qt::AlignCenter );
    connect( advanced, SIGNAL( clicked() ),
             this, SLOT( advancedConfigClicked() ) );

    /* update Frequency Label */

    updateLabel = new QLabel( this );
    text.sprintf( "Update every %d s", updateFrequency );
    updateLabel->setText( text );
    grid->addWidget( updateLabel, 2, 1 );

    /* update Frequency Slider */

    QSlider* updateSlider = new QSlider( QSlider::Horizontal, this );
    updateSlider->setRange( 0, 9 );
    updateSlider->setValue( updateFrequency );
    updateSlider->setTickmarks( QSlider::Both );
    updateSlider->setTickInterval( 1 );
    updateSlider->setSteps( 1, 1 );
    updateSlider->setFocusPolicy( QWidget::NoFocus );
    grid->addWidget( updateSlider, 1, 1 );
    connect( updateSlider, SIGNAL( valueChanged(int) ),
             this, SLOT( updateDelayChange(int) ) );

    setFixedSize( sizeHint() );
    setFocusPolicy( QWidget::NoFocus );

    applet->displayStyleChange( displayStyle );
    applet->updateDelayChange( updateFrequency );

    connect( group, SIGNAL( clicked(int) ),
             this, SLOT( displayStyleChange(int) ) );

    applet->updateDHCPConfig( rocESSID, rocFREQ, rocAP, rocMODE );
}

void WirelessControl::advancedConfigClicked()
{
    AdvancedConfig * a = new AdvancedConfig( this, "dialog", TRUE );
    int result = a->exec();
    a->hide();
    delete a;
    if ( result == QDialog::Accepted )
    {
        readConfig();
        applet->updateDHCPConfig( rocESSID, rocFREQ, rocAP, rocMODE );
    }
}

void WirelessControl::updateDelayChange( int delay )
{
    QString text;
    text.sprintf( "Update every %d s", delay );
    updateLabel->setText( text );
    applet->updateDelayChange( delay );
    writeConfigEntry( "UpdateFrequency", delay );
}

void WirelessControl::displayStyleChange( int style )
{
    applet->displayStyleChange( style );
    writeConfigEntry( "DisplayStyle", style );
}

void WirelessControl::show ( bool )
{
    QPoint curPos = applet->mapToGlobal( QPoint ( 0, 0 ) );

    int w = sizeHint().width();
    int x = curPos.x() - ( w / 2 );

    if ( ( x + w ) > QPEApplication::desktop() ->width() )
        x = QPEApplication::desktop ( ) -> width ( ) - w;

    move( x, curPos.y () - sizeHint().height () );
    QFrame::show();
}

void WirelessControl::readConfig()
{
    Config cfg( "qpe" );
    cfg.setGroup( "Wireless" );

    updateFrequency = cfg.readNumEntry( "UpdateFrequency", 2 );
    displayStyle = cfg.readNumEntry( "DisplayStyle", STYLE_ANTENNA );
    rocESSID = cfg.readBoolEntry( "renew_dhcp_on_essid_change", false );
    rocFREQ = cfg.readBoolEntry( "renew_dhcp_on_freq_change", false );
    rocAP = cfg.readBoolEntry( "renew_dhcp_on_ap_change", false );
    rocMODE = cfg.readBoolEntry( "renew_dhcp_on_mode_change", false );
}

void WirelessControl::writeConfigEntry( const char *entry, int val )
{
    Config cfg( "qpe" );
    cfg.setGroup( "Wireless" );
    cfg.writeEntry( entry, val );
}

//===========================================================================

WirelessApplet::WirelessApplet( QWidget *parent, const char *name )
        : QWidget( parent, name ), visualStyle( STYLE_ANTENNA ),
        timer( 0 ), interface( 0 ), oldiface( 0 ),
        rocESSID( false ), rocFREQ( false ), rocAP( false ), rocMODE( false )
{
    setFixedHeight( 18 );
    setFixedWidth( 14 );
    status = new WirelessControl( this, this, "wireless status" );
}

void WirelessApplet::checkInterface()
{
    interface = 0L;
    ONetwork* net = ONetwork::instance();
    net->synchronize();
    ONetwork::InterfaceIterator it = net->iterator();

    while ( it.current() && !it.current()->isWireless() ) ++it;

    if ( it.current() && it.current()->isWireless() )
        interface = static_cast<OWirelessNetworkInterface*>( it.current() );

    if ( interface )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: using interface '%s'", ( const char* ) interface->name() );
#endif

    }
    else
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: D'oh! No Wireless interface present... :(" );
#endif
        hide();
    }
}

void WirelessApplet::renewDHCP()
{
#ifdef MDEBUG
    qDebug( "WIFIAPPLET: Going to request a DHCP configuration renew." );
#endif

    QString pidfile;
    if ( !interface )
        return ;
    QString ifacename( interface->name() );

    // At first we are trying dhcpcd

    pidfile.sprintf( "/var/run/dhcpcd-%s.pid", ( const char* ) ifacename );
#ifdef MDEBUG
    qDebug( "WIFIAPPLET: dhcpcd pidfile is '%s'", ( const char* ) pidfile );
#endif
    int pid;
    QFile pfile( pidfile );
    bool hasFile = pfile.open( IO_ReadOnly );
    QTextStream s( &pfile );
    if ( hasFile )
    {
        s >> pid;
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: sent SIGALARM to pid %d", pid );
#endif
        kill( pid, SIGALRM );
        return ;
    }

    // No dhcpcd, so we are trying udhcpc
#ifdef MDEBUG
    qDebug( "WIFIAPPLET: dhcpcd not available." );
#endif
    pidfile.sprintf( "/var/run/udhcpc.%s.pid", ( const char* ) ifacename );
#ifdef MDEBUG
    qDebug( "WIFIAPPLET: udhcpc pidfile is '%s'", ( const char* ) pidfile );
#endif
    QFile pfile2( pidfile );
    hasFile = pfile2.open( IO_ReadOnly );
    QTextStream s2( &pfile2 );
    if ( hasFile )
    {
        s2 >> pid;
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: sent SIGUSR1 to pid %d", pid );
#endif
        kill( pid, SIGUSR1 );
        return ;
    }
}

void WirelessApplet::updateDHCPConfig( bool ESSID, bool FREQ, bool AP, bool MODE )
{
    rocESSID = ESSID;
    rocFREQ = FREQ;
    rocAP = AP;
    rocMODE = MODE;
}

void WirelessApplet::updateDelayChange( int delay )
{
    if ( timer )
        killTimer( timer );
    delay *= 1000;
    if ( delay == 0 )
        delay = 50;
    timer = startTimer( delay );
}

void WirelessApplet::displayStyleChange( int style )
{
    visualStyle = style;
    repaint();
}

WirelessApplet::~WirelessApplet()
{}

void WirelessApplet::timerEvent( QTimerEvent* )
{
#ifdef MDEBUG
    qDebug( "WirelessApplet::timerEvent" );
#endif
    if ( interface )
    {
        if ( !ONetwork::instance()->isPresent( (const char*) interface->name() ) )
        {
#ifdef MDEBUG
            qDebug( "WIFIAPPLET: Interface no longer present." );
#endif
            interface = 0L;
            mustRepaint();
            return;
        }

        if ( mustRepaint() )
        {
#ifdef MDEBUG
            qDebug( "WIFIAPPLET: A value has changed -> repainting." );
#endif
            repaint();
        }

        if ( status->isVisible() )
        {
            updatePopupWindow();
        }
    }
    else
    {
        checkInterface();
    }
}

void WirelessApplet::mousePressEvent( QMouseEvent * )
{
    if ( status->isVisible() )
        status->hide();
    else
        status->show( true );
}

bool WirelessApplet::mustRepaint()
{
    // check if there are enough changes to justify a (flickering) repaint

    // has the interface changed?

    if ( interface != oldiface )
    {
        oldiface = interface;
        if ( interface )
        {
#ifdef MDEBUG
            qDebug( "WIFIAPPLET: We had no interface but now we have one! :-)" );
#endif
            show();
        }
        else
        {
#ifdef MDEBUG
            qDebug( "WIFIAPPLET: We had a interface but now we don't have one! ;-(" );
#endif
            hide();
            return true;
        }
    }

    const char** pixmap = getQualityPixmap();

    if ( pixmap && ( pixmap != oldpixmap ) )
    {
        oldpixmap = pixmap;
        return true;
    }

    int noiseH = 50; // iface->noisePercent() * ( height() - 3 ) / 100;
    int signalH = interface->signalStrength() * ( height() - 3 ) / 100;
    int qualityH = 50; // iface->qualityPercent() * ( height() - 3 ) / 100;

    if ( ( noiseH != oldnoiseH )
            || ( signalH != oldsignalH )
            || ( qualityH != oldqualityH ) )
    {
        oldnoiseH = noiseH;
        oldsignalH = signalH;
        oldqualityH = qualityH;
        return true;
    }

    if ( rocESSID && ( oldESSID != interface->SSID() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: ESSID has changed." );
#endif
        renewDHCP();
    }
    else if ( rocFREQ && ( oldFREQ != interface->frequency() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: FREQ has changed." );
#endif
        renewDHCP();
    }
    else if ( rocAP && ( oldAP != interface->associatedAP().toString() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: AP has changed." );
#endif
        renewDHCP();
    }
    else if ( rocMODE && ( oldMODE != interface->mode() ) )
    {
#ifdef MDEBUG
        qDebug( "WIFIAPPLET: MODE has changed." );
#endif
        renewDHCP();
    }

    oldESSID = interface->SSID();
    oldMODE = interface->mode();
    oldFREQ = interface->frequency();
    oldAP = interface->associatedAP().toString();

    return false;
}

void WirelessApplet::updatePopupWindow()
{
    int qualityH = interface->signalStrength();

    if ( status->mgraph )
        status->mgraph->addValue( qualityH, false );

    QString freqString;
    QString cell = ( interface->mode() == "Managed" ) ? "AP: " : "Cell: ";
    freqString.sprintf( "%.3f GHz", interface->frequency() );
    status->statusLabel->setText( "Station: " + interface->nickName() + "<br>" +
                                  "ESSID: " + interface->SSID() + "<br>" +
                                  "MODE: " + interface->mode() + "<br>" +
                                  "FREQ: " + freqString + "<br>" +
                                  cell + " " + interface->associatedAP().toString() );
}

const char** WirelessApplet::getQualityPixmap()
{
    if ( !interface ) return ( const char** ) nowireless_xpm;
    int qualityH = interface->signalStrength();
    if ( qualityH < 0 ) return ( const char** ) nowireless_xpm;

    if ( visualStyle == STYLE_ANTENNA )
    {
        if ( qualityH < 1 ) return ( const char** ) connect0_xpm;
        if ( qualityH < 17 ) return ( const char** ) connect1_xpm;
        if ( qualityH < 34 ) return ( const char** ) connect2_xpm;
        if ( qualityH < 50 ) return ( const char** ) connect3_xpm;
        if ( qualityH < 65 ) return ( const char** ) connect4_xpm;
        return ( const char** ) connect5_xpm;
    }

    return 0; // please draw your bars
}

void WirelessApplet::paintEvent( QPaintEvent* )
{
    QPainter p( this );
    QColor color;

    const char** pixmap = getQualityPixmap();

    if ( pixmap )
        p.drawPixmap( 0, 1, pixmap );
    else
    {

        int noiseH = 30; // iface->noisePercent() * ( height() - 3 ) / 100;
        int signalH = 50; // iface->signalPercent() * ( height() - 3 ) / 100;
        int qualityH = interface->signalStrength(); // iface->qualityPercent() * ( height() - 3 ) / 100;

        double intensity;
        int pixelHeight;
        int pixelWidth = 2;
        int Hue;
        int barSpace = 3;
        int leftoffset = 0;
        int bottomoffset = 2;

        // draw noise indicator
        pixelHeight = noiseH;
        Hue = 50;
        for ( int i = 0; i < pixelHeight; ++i )
        {
            intensity = 50 + ( ( double ) i / ( double ) pixelHeight ) * 205;
            color.setHsv( Hue, 255, intensity );
            p.setPen ( color );
            p.drawLine( leftoffset, height() - bottomoffset - i, pixelWidth + leftoffset, height() - bottomoffset - i );
        }

        // draw signal indicator
        pixelHeight = signalH;
        Hue = 100;
        leftoffset += pixelWidth + barSpace;
        for ( int i = 0; i < pixelHeight; ++i )
        {
            intensity = 50 + ( ( double ) i / ( double ) pixelHeight ) * 205;
            color.setHsv( Hue, 255, intensity );
            p.setPen ( color );
            p.drawLine( leftoffset, height() - bottomoffset - i, pixelWidth + leftoffset, height() - bottomoffset - i );
        }

        // draw quality indicator
        pixelHeight = qualityH;
        Hue = 250;
        leftoffset += pixelWidth + barSpace;
        for ( int i = 0; i < pixelHeight; ++i )
        {
            intensity = 50 + ( ( double ) i / ( double ) pixelHeight ) * 205;
            color.setHsv( Hue, 255, intensity );
            p.setPen ( color );
            p.drawLine( leftoffset, height() - bottomoffset - i, pixelWidth + leftoffset, height() - bottomoffset - i );
        }
    }
}


int WirelessApplet::position()
{
    return 6;
}


Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( OTaskbarAppletWrapper<WirelessApplet> );
}

