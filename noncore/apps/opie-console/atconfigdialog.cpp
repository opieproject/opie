
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qscrollview.h>

#include "atconfigdialog.h"
#include "io_modem.h"


ATConfigDialog::ATConfigDialog(  QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ) {


    setCaption( tr( "Dialing parameter setup" ) );

    QVBoxLayout *layout = new QVBoxLayout( this );
    QTabWidget *tabWidget = new QTabWidget( this );

    tabWidget->addTab( tab0( this ), tr("Settings1") );
    tabWidget->addTab( tab1( this ), tr("Settings2") );

    layout->addWidget( tabWidget );

}

QWidget* ATConfigDialog::tab0( QWidget* parent) {


    QScrollView* sv = new QScrollView( parent );

    QWidget *returnWidget = new QWidget( sv->viewport() );
    sv->setResizePolicy( QScrollView::AutoOneFit );
//  sv->setHScrollBarMode( QScrollView::AlwaysOff );
//    sv->setFrameShape( QFrame::NoFrame );




    QGridLayout *layout = new QGridLayout( returnWidget, 10, 2 );

    QLabel *initStringLabel = new QLabel( tr("Init string "),  returnWidget );
    initStringLine = new QLineEdit( returnWidget );
    layout->addWidget( initStringLabel, 0, 0 );
    layout->addWidget( initStringLine, 0, 1 );

    QLabel *resetStringLabel = new QLabel( tr("Reset string "), returnWidget );
    resetStringLine = new QLineEdit( returnWidget );
    layout->addWidget( resetStringLabel, 1, 0 );
    layout->addWidget( resetStringLine, 1, 1 );

    QLabel *dialPref1Label = new QLabel( tr("Dialing prefix #1 " ), returnWidget );
    dialPref1Line = new QLineEdit( returnWidget );
    layout->addWidget( dialPref1Label, 2, 0 );
    layout->addWidget( dialPref1Line, 2, 1 );

    QLabel *dialSuf1Label = new QLabel( tr("Dialing suffix #1 " ), returnWidget );
    dialSuf1Line = new QLineEdit( returnWidget );
    layout->addWidget( dialSuf1Label, 3, 0 );
    layout->addWidget( dialSuf1Line, 3, 1 );

    QLabel *dialPref2Label = new QLabel( tr("Dialing prefix #2 " ), returnWidget );
    dialPref2Line = new QLineEdit( returnWidget );
    layout->addWidget( dialPref2Label, 4, 0 );
    layout->addWidget( dialPref2Line, 4, 1 );

    QLabel *dialSuf2Label = new QLabel( tr("Dialing suffix #2 " ), returnWidget );
    dialSuf2Line = new QLineEdit( returnWidget );
    layout->addWidget( dialSuf2Label, 5, 0 );
    layout->addWidget( dialSuf2Line, 5, 1 );

    QLabel *dialPref3Label = new QLabel( tr("Dialing prefix #3 " ), returnWidget );
    dialPref3Line = new QLineEdit( returnWidget );
    layout->addWidget( dialPref3Label, 6, 0 );
    layout->addWidget( dialPref3Line, 6, 1 );

    QLabel *dialSuf3Label = new QLabel( tr("Dialing suffix #3 " ), returnWidget );
    dialSuf3Line = new QLineEdit( returnWidget );
    layout->addWidget( dialSuf3Label, 7, 0 );
    layout->addWidget( dialSuf3Line, 7, 1 );

    QLabel *connectLabel = new QLabel( tr("Connect string " ), returnWidget );
    connectLine = new QLineEdit( returnWidget );
    layout->addWidget( connectLabel, 8, 0 );
    layout->addWidget( connectLine, 8, 1 );

    QLabel *hangupLabel = new QLabel( tr("Hang-up string " ), returnWidget );
    hangupLine  = new QLineEdit( returnWidget );
    layout->addWidget( hangupLabel, 9, 0 );
    layout->addWidget( hangupLine, 9, 1 );

    sv->addChild( returnWidget );

    return sv;

}

QWidget* ATConfigDialog::tab1( QWidget* parent ) {


    QWidget *returnWidget = new QWidget( parent );

    QGridLayout *layout = new QGridLayout( returnWidget, 8, 2 );

    QLabel *dialTimeLabel = new QLabel( tr("Dial time " ), returnWidget );
    dialTimeSpin = new QSpinBox( returnWidget );
    layout->addWidget( dialTimeLabel, 0, 0 );
    layout->addWidget( dialTimeSpin, 0, 1 );

    QLabel *delayRedialLabel = new QLabel( tr("Delay before redial " ), returnWidget );
    delayRedialSpin = new QSpinBox( returnWidget );
    layout->addWidget( delayRedialLabel, 1, 0 );
    layout->addWidget( delayRedialSpin, 1, 1 );

    QLabel *numberTriesLabel = new QLabel( tr("Number of tries " ), returnWidget );
    numberTriesSpin = new QSpinBox( returnWidget );
    layout->addWidget( numberTriesLabel, 2, 0 );
    layout->addWidget( numberTriesSpin, 2, 1 );

    QLabel *dtrDropTimeLabel = new QLabel( tr("DTR drop time (0=no) " ), returnWidget );
    dtrDropTimeSpin = new QSpinBox( returnWidget );
    layout->addWidget( dtrDropTimeLabel, 3, 0 );
    layout->addWidget( dtrDropTimeSpin, 3, 1 );

    QLabel *bpsDetectLabel = new QLabel( tr("Auto bps detect " ), returnWidget );
    bpsDetectBox = new QComboBox( returnWidget );
    layout->addWidget( bpsDetectLabel, 4, 0 );
    layout->addWidget( bpsDetectBox, 4, 1 );
    bpsDetectBox->insertItem( tr("No") );
    bpsDetectBox->insertItem( tr("Yes") );

    QLabel *dcdLinesLabel = new QLabel( tr("Modem has DCD line " ), returnWidget );
    dcdLinesBox = new QComboBox( returnWidget );
    layout->addWidget( dcdLinesLabel, 5, 0 );
    layout->addWidget( dcdLinesBox, 5, 1 );
    dcdLinesBox->insertItem( tr("No") );
    dcdLinesBox->insertItem( tr("Yes") );

    QLabel *multiLineUntagLabel = new QLabel( tr("Multi-line untag " ), returnWidget );
    multiLineUntagBox = new QComboBox( returnWidget );
    layout->addWidget( multiLineUntagLabel, 6, 0 );
    layout->addWidget( multiLineUntagBox, 6, 1 );
    multiLineUntagBox->insertItem( tr("No") );
    multiLineUntagBox->insertItem( tr("Yes") );

    return returnWidget;

}


void ATConfigDialog::readConfig( const Profile& config ) {
    qWarning("config in atconfigdialog");

    initStringLine->setText( config.readEntry("InitString", MODEM_DEFAULT_INIT_STRING ) );
    resetStringLine->setText( config.readEntry("ResetString", MODEM_DEFAULT_RESET_STRING ) );
    dialPref1Line->setText( config.readEntry("DialPrefix1", MODEM_DEFAULT_DIAL_PREFIX1 ) );
    dialSuf1Line->setText( config.readEntry("DialSuffix1", MODEM_DEFAULT_DIAL_SUFFIX1 ) );
    dialPref2Line->setText( config.readEntry("DialPrefix2", MODEM_DEFAULT_DIAL_PREFIX1 ) );
    dialSuf2Line->setText( config.readEntry("DialSuffix2", MODEM_DEFAULT_DIAL_SUFFIX1 ) );
    dialPref3Line->setText( config.readEntry("DialPrefix3", MODEM_DEFAULT_DIAL_PREFIX1 ) );
    dialSuf3Line->setText( config.readEntry("DialSuffix3", MODEM_DEFAULT_DIAL_SUFFIX1 ) );
    connectLine->setText( config.readEntry("DefaultConnect", MODEM_DEFAULT_CONNECT_STRING ) );
    hangupLine->setText( config.readEntry("HangupString", MODEM_DEFAULT_HANGUP_STRING ) );
    dialTimeSpin->setValue( config.readNumEntry("DialTime", MODEM_DEFAULT_DIAL_TIME ) );
    delayRedialSpin->setValue( config.readNumEntry("DelayRedial", MODEM_DEFAULT_DELAY_REDIAL ) );
    numberTriesSpin->setValue( config.readNumEntry("NumberTries", MODEM_DEFAULT_NUMBER_TRIES ) );
    dtrDropTimeSpin->setValue( config.readNumEntry("DTRDRopTime", MODEM_DEFAULT_DTR_DROP_TIME ) );
    bpsDetectBox->setCurrentItem( config.readBoolEntry("BPSDetect", MODEM_DEFAULT_BPS_DETECT ) );
    dcdLinesBox->setCurrentItem( config.readBoolEntry("DCDLines", MODEM_DEFAULT_DCD_LINES ) );
    multiLineUntagBox->setCurrentItem( config.readBoolEntry("MultiLineUntag", MODEM_DEFAULT_MULTI_LINE_UNTAG ) );

	// Not implemented yet
	resetStringLine->setEnabled(false);
	dialSuf1Line->setEnabled(false);
	dialPref2Line->setEnabled(false);
	dialSuf2Line->setEnabled(false);
	dialPref3Line->setEnabled(false);
	dialSuf3Line->setEnabled(false);
	dialTimeSpin->setEnabled(false);
	delayRedialSpin->setEnabled(false);
	numberTriesSpin->setEnabled(false);
	dtrDropTimeSpin->setEnabled(false);
	bpsDetectBox->setEnabled(false);
	dcdLinesBox->setEnabled(false);
	multiLineUntagBox->setEnabled(false);
}

void ATConfigDialog::writeConfig( Profile& config ) {

    config.writeEntry( "InitString", initStringLine->text() );
    config.writeEntry( "ResetString", resetStringLine->text() );
    config.writeEntry( "DialPrefix1", dialPref1Line->text() );
    config.writeEntry( "DialSuffix1", dialSuf1Line->text() );
    config.writeEntry( "DialPrefix2", dialPref2Line->text() );
    config.writeEntry( "DialSuffix2",  dialSuf2Line->text() );
    config.writeEntry( "DialPrefix3", dialPref3Line->text() );
    config.writeEntry( "DialSuffix3", dialSuf3Line->text() );
    config.writeEntry( "DefaultConnect", connectLine->text() );
    config.writeEntry( "HangupString", hangupLine->text() );
    config.writeEntry( "DialTime", dialTimeSpin->value() );
    config.writeEntry( "DelayRedial", delayRedialSpin->value() );
    config.writeEntry( "NumberTries", numberTriesSpin->value() );
    config.writeEntry( "DTRDRopTime", dtrDropTimeSpin->value() );
    config.writeEntry( "BPSDetect", bpsDetectBox->currentItem() );
    config.writeEntry( "DCDLines", dcdLinesBox->currentItem() );
    config.writeEntry( "MultiLineUntag", multiLineUntagBox->currentItem() );

}


ATConfigDialog::~ATConfigDialog() {
}
