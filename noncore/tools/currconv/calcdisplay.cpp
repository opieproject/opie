/****************************************************************************
 *
 * File:        calcdisplay.cpp
 *
 * Description:
 *
 *
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *              Paul Eggleton <bluelightning@bluelightning.org>
 *
 * Requirements:    Qt
 *
 ***************************************************************************/

#include <stdio.h>

#include <qvbox.h>
#include <qpixmap.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qmap.h>

#include <opie2/oresource.h>
#include <opie2/oprocess.h>
#include <opie2/oglobal.h>
#include <opie2/oconfig.h>
#include <opie2/odebug.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>

#include "calcdisplay.h"

/* XPM */
static const char *swap_xpm[] = {
/* width height num_colors chars_per_pixel */
"    13    18        2            1",
/* colors */
". c None",
"# c #000000",
/* pixels */
"..#######....",
"..#####......",
"..######.....",
"..#...###....",
"........##...",
".........##..",
"..........##.",
"...........##",
"...........##",
"...........##",
"...........##",
"..........##.",
".........##..",
"........##...",
"..#...###....",
"..######.....",
"..#####......",
"..#######....",
};

#define CURRENCY_DATA_URL  "http://openexchangerates.org"

using namespace Opie::Core;


LCDDisplay::LCDDisplay( QWidget *parent, const char *name )
        : QHBox( parent, name )
{
    this->setMargin(5);
    this->setSpacing(5);

    // Create display
    QVBox *vbxlayout    = new QVBox (this);

    /***************    Top LCD   ***********************/
    grpbxTop    = new QHGroupBox(vbxlayout, "grpbxTop");
    grpbxStyle  = grpbxTop->frameStyle();
    grpbxTop->setMaximumHeight(48);

    cbbxTop     = new QComboBox(grpbxTop, "cbbxTop");
    cbbxTop->setMaximumWidth(50);

    lcdTop      = new QLCDNumber(10, grpbxTop, "lcdTop");
    lcdTop->setMode( QLCDNumber::DEC );
    lcdTop->setSmallDecimalPoint(false);
    lcdTop->setSegmentStyle(QLCDNumber::Flat);

    /**************   Bottom LCD  ************************/
    grpbxBottom = new QHGroupBox(vbxlayout, "grpbxBottom");
    grpbxBottom->setMaximumHeight(46);
    grpbxBottom->setFrameStyle(0);
    grpbxBottom->setFrameShadow(QFrame::MShadow);

    cbbxBottom  = new QComboBox(grpbxBottom, "cbbxBottom");
    cbbxBottom->setMaximumWidth(50);

    lcdBottom   = new QLCDNumber(10, grpbxBottom, "lcdBottom");
    lcdBottom->setMode( QLCDNumber::DEC );
    lcdBottom->setSmallDecimalPoint(false);
    lcdBottom->setSegmentStyle(QLCDNumber::Flat);

    // set combo box signals
    connect(cbbxTop, SIGNAL(activated(int)), this, SLOT(cbbxChange()));
    connect(cbbxBottom, SIGNAL(activated(int)), this, SLOT(cbbxChange()));

    btnSwap     = new QPushButton(this, "swap");
    QPixmap imgSwap((const char**) swap_xpm);
    btnSwap->setPixmap(imgSwap);
    btnSwap->setFixedSize(20,40);
    // set signal
    connect(btnSwap, SIGNAL(clicked()), this, SLOT(swapLCD()));

    QMenuBar *menu = new QMenuBar( this );

    QPopupMenu *currenciesMenu = new QPopupMenu( menu );
    QAction *setting_a = new QAction( tr( "Settings" ),
                             Opie::Core::OResource::loadPixmap( "SettingsIcon", Opie::Core::OResource::SmallIcon ),
                             QString::null, 0, this, 0 );
    connect( setting_a, SIGNAL( activated() ), this, SLOT( slotSettings() ) );
    setting_a->addTo( currenciesMenu );
    QAction *setting_b = new QAction( tr( "Update" ),
                             Opie::Core::OResource::loadPixmap( "down", Opie::Core::OResource::SmallIcon ),
                             QString::null, 0, this, 0 );
    connect( setting_b, SIGNAL( activated() ), this, SLOT( slotUpdate() ) );
    setting_b->addTo( currenciesMenu );
    QAction *setting_c = new QAction( tr( "Info" ),
                             QString::null, 0, this, 0 );
    connect( setting_c, SIGNAL( activated() ), this, SLOT( slotDataInfo() ) );
    setting_c->addTo( currenciesMenu );

    menu->insertItem( tr( "Currencies" ) , currenciesMenu );

    // set default LCD to top
    iCurrentLCD = 0;

    m_updateMode = 0;

    readConfig();
    m_dataDir = QDir::homeDirPath() + "/Applications/currconv/";
    reloadRateData();

    QTimer::singleShot( 0, this, SLOT(slotCheckData()) );
}

LCDDisplay::~LCDDisplay()
{
    saveConfig();
}

/***********************************************************************
 * SLOT: Display value in the correct LCD
 **********************************************************************/
void LCDDisplay::setValue(double dSrcValue)
{

    double  dDstValue=0;

    QString rateFrom;
    QString rateTo;

    // get item index of the focused
    if(!iCurrentLCD) {
        rateFrom = cbbxTop->currentText();
        rateTo = cbbxBottom->currentText();
    }
    else {
        rateFrom = cbbxBottom->currentText();
        rateTo = cbbxTop->currentText();
    }

    if(rateFrom == rateTo) {
        dDstValue = dSrcValue;
    }
    else {
        dDstValue = m_curr.convertRate( rateFrom, rateTo, dSrcValue );
    }


    if(!iCurrentLCD) {
        lcdTop->display(dSrcValue);
        lcdBottom->display(dDstValue);
    }
    else {
        lcdBottom->display(dSrcValue);
        lcdTop->display(dDstValue);
    }

}

/***********************************************************************
 * SLOT: Swap output keypad between LCD displays
 **********************************************************************/
void LCDDisplay::swapLCD(void)
{
    double dCurrentValue;

    // get current value
    if(!iCurrentLCD) {
        // iCurrentLCD = 0, lcdTop has current focus and is going to loose
        // it
        dCurrentValue = lcdTop->value();
        iCurrentLCD = 1;
        grpbxTop->setFrameStyle(0);
        grpbxBottom->setFrameStyle(grpbxStyle);
    }
    else {
        dCurrentValue = lcdBottom->value();
        iCurrentLCD = 0;
        grpbxTop->setFrameStyle(grpbxStyle);
        grpbxBottom->setFrameStyle(0);
    }

    setValue(dCurrentValue);
}

/***********************************************************************
 * SLOT: Currency change
 **********************************************************************/
void LCDDisplay::cbbxChange(void)
{
    double dCurrentValue;

    // get current value
    if(!iCurrentLCD) {
        dCurrentValue = lcdTop->value();
    }
    else {
        dCurrentValue = lcdBottom->value();
    }

    setValue(dCurrentValue);

    m_lastRate1 = cbbxTop->currentText();
    m_lastRate2 = cbbxBottom->currentText();
}

void LCDDisplay::readConfig()
{
    OConfig conf("currconv");
    conf.setGroup("Settings");
    m_visibleRates = QStringList::split(',', conf.readEntry("visibleRates", "EUR,USD,GBP,JPY,HKD,BRL"));
    m_lastRate1 = conf.readEntry("lastRate1");
    m_lastRate2 = conf.readEntry("lastRate2");
    int currBox = conf.readNumEntry("currBox");
    if( currBox ) {
        swapLCD();
    }
}

void LCDDisplay::saveConfig()
{
    OConfig conf("currconv");
    conf.setGroup("Settings");
    conf.writeEntry("lastRate1", m_lastRate1);
    conf.writeEntry("lastRate2", m_lastRate2);
    conf.writeEntry("currBox", QString::number( iCurrentLCD ));
}

void LCDDisplay::slotCheckData()
{
    QString msg = "";
    if( !m_curr.dataLoaded() )
        msg = tr("You need to update the currency conversion data before you can use this application. Do you want to do this now?");
    else if( m_curr.timestamp().daysTo( QDateTime::currentDateTime() ) > 30 )
        msg = tr("Your currency conversion data is more than 30 days old. Do you want to update it now?");

    if( msg != "" ) {
        msg = "<p>" + msg + "</p>";
        if( QMessageBox::warning(this,tr("Currency Converter"),msg,tr("Update"),tr("No thanks"),0,1,0) == 0 )
            slotUpdate();
    }
}

void LCDDisplay::reloadRateData()
{
    m_curr.loadRates( m_dataDir + "latest.json" );
    m_curr.loadCurrencies( m_dataDir + "currencies.json" );
    refreshRates();
}

void LCDDisplay::refreshRates()
{
    QStringList codes = m_curr.rateCodes();
    updateRateCombo(cbbxTop, codes, m_lastRate1);
    updateRateCombo(cbbxBottom, codes, m_lastRate2);
    cbbxChange();
}

void LCDDisplay::updateRateCombo( QComboBox *combo, const QStringList &lst, const QString &dfl )
{
    QString prev = combo->currentText();
    if( prev == "" )
        prev = dfl;
    combo->clear();
    for( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it ) {
        if( m_visibleRates.count() == 0 || m_visibleRates.findIndex(*it) > -1 ) {
            combo->insertItem(*it, -1);
            if( *it == prev )
                combo->setCurrentItem( combo->count() - 1 );
        }
    }
}

void LCDDisplay::slotSettings()
{
    QDialog *dlg = new QDialog( this, 0, true );
    dlg->setCaption(tr("Settings"));
    QVBoxLayout *vbxlayout = new QVBoxLayout(dlg);
    QListView *listview = new QListView(dlg);
    listview->addColumn( tr( "Code" ) );
    listview->addColumn( tr( "Currency" ) );
    listview->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding));
    QStringList codes = m_curr.rateCodes();
    for( QStringList::ConstIterator it = codes.begin(); it != codes.end(); ++it ) {
        QCheckListItem *item = new QCheckListItem( listview, *it, QCheckListItem::CheckBox );
        item->setText(1, m_curr.currencyName(*it));
        if( m_visibleRates.count() == 0 || m_visibleRates.findIndex(*it) > -1 )
            item->setOn(true);
        listview->insertItem( item );
    }
    vbxlayout->addWidget(listview);

    if( QPEApplication::execDialog(dlg) == QDialog::Accepted ) {
        if( listview->childCount() ) {
            m_visibleRates.clear();
            for ( QCheckListItem *i = (QCheckListItem *)listview->firstChild(); i; i = (QCheckListItem *)i->nextSibling() )
                if ( i->isOn() )
                    m_visibleRates += i->text(0);
        }

        OConfig conf("currconv");
        conf.setGroup("Settings");
        conf.writeEntry("visibleRates", m_visibleRates.join(","));

        refreshRates();
    }

    delete dlg;
}

void LCDDisplay::slotUpdate()
{
    if( m_updateMode )
        return;

    OGlobal::createDirPath( m_dataDir );

    startDownload( CURRENCY_DATA_URL "/latest.json", m_dataDir + "latest.json.new", 1 );
}

void LCDDisplay::startDownload(const QString remoteFile, const QString localFile, int updateMode)
{
    OProcess *proc = new OProcess;
    *proc << "wget" << "-N" << "-q" << remoteFile << "-O" << localFile;
    connect( proc, SIGNAL( processExited(Opie::Core::OProcess*) ), this, SLOT( dataRetrieved(Opie::Core::OProcess*) ) );
    m_updateMode = updateMode;
    if ( !proc->start() ) {
        QMessageBox::warning(this, tr("Currconv"), tr("Failed to start download process"));
        m_updateMode = 0;
    }
}

void LCDDisplay::dataRetrieved( Opie::Core::OProcess* proc )
{
    QString msg;
    msg = tr("Currency data update failed!");
    if( proc->normalExit() && proc->exitStatus() == 0 ) {
        if( m_updateMode == 1 ) {
            // Finished downloading rates
            int ret = ::rename( QString(m_dataDir + "latest.json.new").latin1(),
                                QString(m_dataDir + "latest.json").latin1() );
            if( ret == 0 ) {
                delete proc;
                startDownload( CURRENCY_DATA_URL "/currencies.json", m_dataDir + "currencies.json.new", 2 );
                return;
            }
        }
        else if( m_updateMode == 2 ) {
            // Finished downloading currencies
            int ret = ::rename( QString(m_dataDir + "currencies.json.new").latin1(),
                                QString(m_dataDir + "currencies.json").latin1() );
            if( ret == 0 ) {
                reloadRateData();
                msg = tr("Currency data updated successfully");
            }
        }
    }
    QCopEnvelope e("QPE/TaskBar", "message(QString,QString)");
    e << msg;
    e << QString("currconv");
    delete proc;
    m_updateMode = 0;
}

void LCDDisplay::slotDataInfo()
{
    QString msg;
    msg += "<p><b>" + tr("source") + "</b>: " CURRENCY_DATA_URL "</p>";
    if( m_curr.dataLoaded() ) {
        QMap<QString,QString> extra = m_curr.extraInfo();
        for( QMap<QString,QString>::Iterator it = extra.begin(); it != extra.end(); ++it ) {
            msg += "<p><b>" + it.key() + "</b>: " + it.data() + "</p>";
        }
        msg += "<p><b>" + tr("last updated") + "</b>: " + m_curr.timestamp().toString() + "</p>";
    }
    else
        msg += "<p>" + tr("No data loaded, select Currencies -> Update to download latest data") + "</p>";
    QMessageBox::information(this, tr("Currency Converter"), msg );
}
