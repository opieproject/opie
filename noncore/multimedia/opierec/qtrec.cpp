/****************************************************************************
 // qtrec.cpp
 Created: Thu Jan 17 11:19:58 2002
 copyright 2002 by L.J. Potter <ljp@llornkcor.com>
****************************************************************************/

#include "pixmaps.h"
#include "qtrec.h"
#include "waveform.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <opie2/osounddevice.h>
#include <qpe/config.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/storage.h>
#include <qpe/filemanager.h>
using namespace Opie::Core;

/* QT */
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qdir.h>

/* STD */
#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <unistd.h>

static int deviceSampleRates[] = { 11025, 16000, 22050, 32000, 44100, 48000, -1 };
static int deviceBitRates[] = { 8, 16, -1 };

// FIXME remove this structure
typedef struct {
    int sampleRate;
    int SecondsToRecord; // number of seconds that should be recorded
    float numberOfRecordedSeconds; //total number of samples recorded
    QString fileName; //name of fiel to be played/recorded
} fileParameters;

fileParameters filePara;

int mode;
QLabel *timeLabel;
QSlider *timeSlider;

Waveform* waveform;

void QtRec::playIt()
{

    m_player.play( this );

}

void QtRec::playerCallback(const char *buffer, const int bytes, int &position, bool &stopflag, bool &paused)
{
    stopflag = ( mode == MODE_STOPPING );
    paused = ( mode == MODE_PAUSED );
    if( !paused ) {
        waveform->newSamples( buffer, bytes );
        timeSlider->setValue( position );
    }
    
    qApp->processEvents();
    position = timeSlider->value();
}

QtRec::QtRec( QWidget* parent,  const char* name, WFlags fl )
        : QWidget( parent, name, fl )
{
    if ( !name )
        setName( "OpieRec" );

    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    m_dirPath = cfg.readEntry("directory", QPEApplication::documentDir()) + "/";

    mode = MODE_IDLE;

    init();
    initConfig();
    initConnections();
    renameBox = 0;

// open sound device to get volumes
    Config hwcfg("OpieRec");
    hwcfg.setGroup("Hardware");

    // FIXME this is broken
    OSoundDevice *soundDevice = new OSoundDevice( "default" );

    getInVol();
    getOutVol();

    soundDevice->closeDevice();
    delete soundDevice;
    QTimer::singleShot(100,this, SLOT(initIconView()));

    if( autoMute)
        doMute( true);
//    ListView1->setFocus();
}

QtRec::~QtRec() {

}

void QtRec::hideEvent( QHideEvent *e ) {
    QWidget::hideEvent( e );
    stop();
}

void QtRec::cleanUp() {

//X    if( !stopped) {
//X        stopped = true;
//X        endRecording();
//X    }

    ListView1->clear();

    if( autoMute)
        doMute(false);
}

void QtRec::init() {

    QPixmap image6( ( const char** ) image6_data );

    setCaption( tr( "OpieRecord " ));
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 2);
    layout->setMargin( 2);

    TabWidget = new QTabWidget( this, "TabWidget" );
    layout->addMultiCellWidget(TabWidget, 0, 7, 0, 8);
//    TabWidget->setTabShape(QTabWidget::Triangular);

    ///**********<<<<<<<<<<<<>>>>>>>>>>>>***************
    tab = new QWidget( TabWidget, "tab" );

    QGridLayout *layout1 = new QGridLayout( tab);
    layout1->setSpacing( 2);
    layout1->setMargin( 2);

    timeSlider = new QSlider( 0,100,10,0, QSlider::Horizontal, tab, (const char *) "timeSlider" );
    layout1->addMultiCellWidget( timeSlider, 1, 1, 0, 3);

//     timeLabel = new QLabel( tab, "TimeLabel" );
//     layout1->addMultiCellWidget( timeLabel, 0, 0, 0, 3);

//     playLabel2 = new QLabel(tab, "PlayLabel2" );
//     playLabel2->setText(tr("Play") );
//     playLabel2->setFixedHeight( 18);
//     layout1->addMultiCellWidget( playLabel2, 0, 0, 4, 4);

    Stop_PushButton = new QPushButton(  tab, "Stop_PushButton" );
    layout1->addMultiCellWidget( Stop_PushButton, 1, 1, 4, 4);
    Stop_PushButton->setFixedSize( 22, 22);
    Stop_PushButton->setPixmap( Opie::Core::OResource::loadPixmap("play2", Opie::Core::OResource::SmallIcon) );

    toBeginningButton = new QPushButton(  tab, "Beginning_PushButton" );
    layout1->addMultiCellWidget(toBeginningButton, 1, 1, 5, 5);
    toBeginningButton->setFixedSize( 22, 22);
    toBeginningButton->setPixmap( Opie::Core::OResource::loadPixmap("fastback", Opie::Core::OResource::SmallIcon) );

    toEndButton = new QPushButton(  tab, "End_PushButton" );
    layout1->addMultiCellWidget( toEndButton, 1, 1, 6, 6);
    toEndButton->setFixedSize( 22, 22);
    toEndButton->setPixmap(  Opie::Core::OResource::loadPixmap( "fastforward", Opie::Core::OResource::SmallIcon )  );

//     QLabel *recLabel2;
//     recLabel2 = new QLabel( tab, "recLabel2" );
//     recLabel2->setText(tr("Rec"));
//     recLabel2->setFixedHeight( 18);
//     layout1->addMultiCellWidget( recLabel2, 0, 0, 7, 7);

    Rec_PushButton = new QPushButton( tab, "Rec_PushButton" );
    layout1->addMultiCellWidget(  Rec_PushButton, 1, 1, 7, 7);
    Rec_PushButton->setFixedSize( 22, 22);
    Rec_PushButton->setPixmap( image6 );

    rewindTimer = new QTimer( this );
    connect( rewindTimer, SIGNAL( timeout() ),
             this, SLOT( rewindTimerTimeout() ) );

    forwardTimer = new QTimer( this );
    connect( forwardTimer, SIGNAL( timeout() ),
             this, SLOT( forwardTimerTimeout() ) );

    deleteSoundButton  = new QPushButton( tab, "deleteSoundButton" );
    layout1->addMultiCellWidget( deleteSoundButton, 1, 1, 8, 8);
    deleteSoundButton->setFixedSize( 22, 22);
    deleteSoundButton->setPixmap(  Opie::Core::OResource::loadPixmap( "editdelete", Opie::Core::OResource::SmallIcon )  );

    ListView1 = new QListView( tab, "IconView1" );
    layout1->addMultiCellWidget( ListView1, 2, 2, 0, 8);

    ListView1->addColumn( tr( "Name" ) );
    ListView1->setSorting( 1, false);
    ListView1->addColumn( tr( "Time" ) ); //in seconds
    ListView1->addColumn( tr( "Size" ) );
    ListView1->setColumnWidthMode(0, QListView::Maximum);
    ListView1->setColumnAlignment( 1, QListView::AlignRight);
    ListView1->setColumnAlignment( 2, QListView::AlignRight);
    ListView1->setAllColumnsShowFocus( true );
    QPEApplication::setStylusOperation( ListView1->viewport(), QPEApplication::RightOnHold);

    TabWidget->insertTab( tab, tr( "Files" ) );

    ///**********<<<<<<<<<<<<>>>>>>>>>>>>***************
    tab_3 = new QWidget( TabWidget, "tab_3" );
    QGridLayout *glayout3 = new QGridLayout( tab_3 );
    glayout3->setSpacing( 2);
    glayout3->setMargin( 2);
    ////////////////////////////////////
    sampleGroup = new QGroupBox( tab_3, "samplegroup" );
    sampleGroup->setTitle( tr( "Sample Rate" ) );
    sampleGroup->setFixedSize( 95,50);

    sampleRateComboBox = new QComboBox( false, sampleGroup, "SampleRateComboBox" );
    sampleRateComboBox->setGeometry( QRect( 10, 20, 80, 25 ) );
    QString s;
    int z = 0;
    while( deviceSampleRates[z] != -1) {
        sampleRateComboBox->insertItem( s.setNum( deviceSampleRates[z], 10));
        z++;
    }


    glayout3->addMultiCellWidget(  sampleGroup, 0, 0, 0, 0);

    sizeGroup= new QGroupBox( tab_3, "sizeGroup" );
    sizeGroup->setTitle( tr( "Limit Size" ) );
    sizeGroup->setFixedSize( 80, 50);

    sizeLimitCombo = new QComboBox( false, sizeGroup, "sizeLimitCombo" );
    sizeLimitCombo ->setGeometry( QRect( 5, 20, 70, 25 ) );
    sizeLimitCombo->insertItem(tr("Unlimited"));

    for(int i=1;i<13; i++) {
        sizeLimitCombo->insertItem( QString::number( i * 5));
    }


    glayout3->addMultiCellWidget(  sizeGroup, 0, 0, 1, 1);
    dirGroup = new QGroupBox( tab_3, "dirGroup" );
    dirGroup->setTitle( tr( "File Directory" ) );
    dirGroup->setFixedSize( 130, 50);

    directoryComboBox = new QComboBox( false, dirGroup, "dirGroup" );
    directoryComboBox->setGeometry( QRect( 10, 15, 115, 25 ) );

    glayout3->addMultiCellWidget(  dirGroup, 1, 1, 0, 0);

    bitGroup = new QGroupBox( tab_3, "bitGroup" );
    bitGroup->setTitle( tr( "Bit Depth" ) );
    bitGroup->setFixedSize( 65, 50);

    bitRateComboBox = new QComboBox( false, bitGroup, "BitRateComboBox" );

    z = 0;
    while( deviceBitRates[z] != -1) {
        bitRateComboBox->insertItem( s.setNum( deviceBitRates[z], 10) );
        z++;
    }

    bitRateComboBox->setGeometry( QRect( 5, 20, 50, 25 ) );

    glayout3->addMultiCellWidget(  bitGroup, 1, 1, 1, 1);

    compressionCheckBox = new QCheckBox ( tr("Wave Compression (smaller files)"), tab_3 );

    autoMuteCheckBox = new QCheckBox ( tr("Auto Mute"), tab_3 );
    stereoCheckBox = new QCheckBox ( tr("Stereo"), tab_3 );

    glayout3->addMultiCellWidget( compressionCheckBox, 2, 2, 0, 3);
    glayout3->addMultiCellWidget( autoMuteCheckBox, 3, 3, 0, 0);
    glayout3->addMultiCellWidget( stereoCheckBox, 3, 3, 1, 1);

    tab_5 = new QWidget( TabWidget, "tab_5" );

    QHBoxLayout *Layout19a;
    Layout19a = new QHBoxLayout( tab_5);
    Layout19a->setSpacing( 2 );
    Layout19a->setMargin( 0 );

    Layout15 = new QVBoxLayout( this);
    Layout15->setSpacing( 2 );
    Layout15->setMargin( 0 );

    Layout15b = new QVBoxLayout( this);
    Layout15b->setSpacing( 2 );
    Layout15b->setMargin( 0 );

    TextLabel2 = new QLabel( tab_5, "InputLabel" );
    TextLabel2->setText( tr( "In"));
    TextLabel2->setFixedWidth( 35);
    Layout15->addWidget( TextLabel2 );

    TextLabel3 = new QLabel( tab_5, "OutputLabel" );
    TextLabel3->setText( tr( "Out" ) );
    Layout15b->addWidget( TextLabel3 );

    InputSlider = new QSlider( -100, 0, 10, 0, QSlider::Vertical, tab_5, (const char *) "InputSlider" );
    InputSlider->setTickmarks( QSlider::Both);
    Layout15->addWidget( InputSlider);

    OutputSlider = new QSlider( -100,0,10,0, QSlider::Vertical,tab_5,(const char *) "OutputSlider" );
    OutputSlider->setTickmarks( QSlider::Both);

    Layout15b->addWidget( OutputSlider );

    outMuteCheckBox = new QCheckBox ( tr("mute"), tab_5 );
    Layout15->addWidget( outMuteCheckBox );

    inMuteCheckBox = new QCheckBox ( tr("mute"), tab_5 );
    inMuteCheckBox-> setFocusPolicy ( QWidget::NoFocus );
    Layout15b->addWidget( inMuteCheckBox );


    Layout19a->addLayout( Layout15 );
    Layout19a->addLayout( Layout15b );

    fillDirectoryCombo();

    TabWidget->insertTab( tab_3, tr( "Options" ) );

    TabWidget->insertTab( tab_5, tr( "Volume" ) );

    waveform = new Waveform( this, "waveform" );
    waveform->setMinimumSize( QSize( 0, 50 ) );

    layout->addMultiCellWidget( waveform, 8, 8, 0, 8);
    waveform->setBackgroundColor ( black );

    QCopChannel *chan = new QCopChannel( "QPE/Recorder", this );
    connect( chan, SIGNAL(received(const QCString&,const QByteArray&)),
            this, SLOT(receive(const QCString&,const QByteArray&)) );
    connect( qApp, SIGNAL( appMessage(const QCString&,const QByteArray&) ),
             this, SLOT( receive(const QCString&,const QByteArray&) ) );

    QCopChannel *chan2 = new QCopChannel("QPE/System", this);
    connect(chan2, SIGNAL(received(const QCString&,const QByteArray&)),
        this, SLOT(systemMsg(const QCString&,const QByteArray&)) );
}

void QtRec::initIconView() {
    ListView1->clear();
    QString temp;
    QPixmap image0( ( const char** ) image0_data );

    QString lastFile = getSelectedFile();

    QDir snddir(m_dirPath);
    QStringList sndlist = snddir.entryList( "*.wav", QDir::Files|QDir::Readable,
                                                    QDir::Name );
    for ( QStringList::Iterator it = sndlist.begin(); it != sndlist.end(); it++ )
    {
        QListViewItem * item;
        QString fileS, mediaLocation, fileDate,  filePath;

        filePath = m_dirPath + (*it);
        QFileInfo info(filePath);
        fileDate = info.lastModified().toString();

        QString sizeStr;
        fileSize(info.size(), sizeStr);

        item = new QListViewItem( ListView1, (*it), fileS, sizeStr );
        item->setPixmap( 0, image0);
        if( lastFile == filePath)
            ListView1->setSelected( item, true);
    }

    setButtons();
}

void QtRec::initConnections() {
    connect(qApp,SIGNAL(aboutToQuit()),SLOT(cleanUp()));

    connect(toBeginningButton,SIGNAL(pressed()),this,SLOT(rewindPressed()));
    connect(toBeginningButton,SIGNAL(released()),this,SLOT(rewindReleased()));
    connect(toEndButton,SIGNAL(pressed()),this,SLOT(FastforwardPressed()));
    connect(toEndButton,SIGNAL(released()),this,SLOT(FastforwardReleased()));

    connect(deleteSoundButton,SIGNAL(released()),this,SLOT(deleteSound()));

    connect(Stop_PushButton,SIGNAL(released()),this,SLOT(doPlayBtn()));

    connect(Rec_PushButton,SIGNAL(released()),this,SLOT(newSound()));

    connect(TabWidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(thisTab(QWidget*)));

    connect(OutputSlider,SIGNAL(sliderReleased()),this,SLOT(changedOutVolume()));
    connect(InputSlider,SIGNAL(sliderReleased()),this,SLOT(changedInVolume()));

    connect(sampleRateComboBox,SIGNAL(activated(int)),this,SLOT(changesamplerateCombo(int)));
    connect(bitRateComboBox,SIGNAL(activated(int)),this,SLOT(changebitrateCombo(int)));
    connect(directoryComboBox,SIGNAL(activated(int)),this,SLOT(changeDirCombo(int)));
    connect(sizeLimitCombo,SIGNAL(activated(int)),this,SLOT(changeSizeLimitCombo(int)));

    connect(stereoCheckBox,SIGNAL(toggled(bool)),this,SLOT(changeStereoCheck(bool)));
    connect(outMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(doVolMuting(bool)));
    connect(inMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(doMicMuting(bool)));

    connect(ListView1,SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint&,int)),this,SLOT(listPressed(int,QListViewItem*,const QPoint&,int)));

    connect(timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(changeTimeSlider(int)));
    connect(timeSlider,SIGNAL(sliderPressed()),this,SLOT(timeSliderPressed()));
    connect(timeSlider,SIGNAL(sliderReleased()),this,SLOT(timeSliderReleased()));

    connect(compressionCheckBox,SIGNAL(toggled(bool)),this,SLOT(compressionSelected(bool)));
    connect(autoMuteCheckBox,SIGNAL(toggled(bool)),this,SLOT(slotAutoMute(bool)));
}

void QtRec::initConfig() {
    int index, fred, i;
    Config cfg("OpieRec");
    cfg.setGroup("Settings");

    index = cfg.readNumEntry("samplerate",22050);
    bool ok;

    for(int ws=0;ws<sampleRateComboBox->count();ws++) {
        fred = sampleRateComboBox->text(ws).toInt(&ok, 10);
        if( index == fred) {
            sampleRateComboBox->setCurrentItem(ws);
        }
    }

    i = cfg.readNumEntry("bitrate",16);
    if(i == 16)
        bitRateComboBox->setCurrentItem( 1);
    else    if(i == 24)
        bitRateComboBox->setCurrentItem( 2);
    else    if(i == 32)
        bitRateComboBox->setCurrentItem( 3);
    else
        bitRateComboBox->setCurrentItem( 0);

    i = cfg.readNumEntry("sizeLimit", 5 );
    QString temp;
    sizeLimitCombo->setCurrentItem((i/5));

    stereoCheckBox->setChecked( cfg.readBoolEntry("stereo", 0));

    compressionCheckBox->setChecked( cfg.readBoolEntry("wavCompression",1));
    if( compressionCheckBox->isChecked()) {
        bitRateComboBox->setCurrentItem(1);
        bitRateComboBox->setEnabled(false);
    }

    autoMuteCheckBox->setChecked( cfg.readBoolEntry("useAutoMute",0));
    if( autoMuteCheckBox->isChecked())
        slotAutoMute(true);
    else
        slotAutoMute(false);

    Config cofg( "qpe");
    cofg.setGroup( "Volume");
    outMuteCheckBox->setChecked( cofg.readBoolEntry( "Mute",0));
    inMuteCheckBox->setChecked( cofg.readBoolEntry( "MicMute",0));
}

void QtRec::stop() {
    odebug << "stop called" << oendl;
    // Signal player loop that it's time to stop
    mode = MODE_STOPPING;
}

void QtRec::doPlayBtn() {
    if( mode != MODE_IDLE ) {
        stop();
    }
    else {
        if(ListView1->currentItem() == 0) return;
        start();
    }
}

void QtRec::start() {
    if( mode == MODE_IDLE ) {
        mode = MODE_PLAYING;
        setButtons();
        secCount = 1;
        setFocus();

        if( openPlayFile())
            if( setupAudio( false))  //recording is false
                doPlay();
    }
}

bool QtRec::rec() { //record
//    QString timeString;
//    timeString.sprintf("%.0f",  0.0);
//    timeLabel->setText( timeString+ " seconds");
    secCount = 1;

    OWavFileParameters fileparams;
    bool ok;

    fileparams.resolution = bitRateComboBox->currentText().toInt( &ok,10);

    if(stereoCheckBox->isChecked())
        fileparams.channels = 2;
    else
        fileparams.channels = 1;

    if(compressionCheckBox->isChecked())
        fileparams.format = WAVE_FORMAT_DVI_ADPCM;
    else
        fileparams.format = WAVE_FORMAT_PCM;

    fileparams.sampleRate = sampleRateComboBox->currentText().toInt( &ok,10);

    setupFileName();
    if( m_recorder.setup( filePara.fileName, fileparams ) ) {
        int fileSize = 0;
        Config cfg("OpieRec");
        cfg.setGroup("Settings");
        filePara.SecondsToRecord = getCurrentSizeLimit();
        int diskSize = checkDiskSpace( m_recorder.getWriteFileName() );

        if( filePara.SecondsToRecord == 0) {
            fileSize = diskSize;
        }
        else if( fileparams.format == WAVE_FORMAT_PCM) {
//                odebug << "WAVE_FORMAT_PCM" << oendl;
            fileSize = (filePara.SecondsToRecord ) * fileparams.channels
                * fileparams.sampleRate * ( fileparams.resolution / 8) + 1000;
        }
        else {
//                odebug << "WAVE_FORMAT_DVI_ADPCM" << oendl;
            fileSize = ((filePara.SecondsToRecord) * fileparams.channels
                        * fileparams.sampleRate * ( fileparams.resolution / 8) ) / 4 + 250;
        }

        timeSlider->setRange(0, filePara.SecondsToRecord);

        if( diskSize < fileSize/1024) {
            QMessageBox::warning(this,
                                    tr("Low Disk Space"),
                                    tr("You are running low of\nrecording space\n"
                                    "or a card isn't being recognized"));
            stop();
        }
        else {
            waveform->changeSettings( fileparams.sampleRate, fileparams.channels, m_recorder.getDeviceFormat() );

            odebug << "Start recording" << oendl;
            mode = MODE_RECORDING;
            setButtons();

            startTimer(1000);
            m_recorder.record( this );
            endRecording();
        }
    }
    return true;
}

void QtRec::recorderCallback(const char *buffer, const int bytes, const int totalbytes, bool &stopflag)
{
    if( mode != MODE_RECORDING )
        stopflag = true;
    else {
        waveform->newSamples( buffer, bytes );
        qApp->processEvents();
    }
}

/*
  This happens when a tab is selected*/
void QtRec::thisTab(QWidget* widg) {
    if(widg != NULL) {
        int index = TabWidget->currentPageIndex();

        if(index == 0) { //file page
        }

        if(index == 1) { //control page
            fillDirectoryCombo();
        }

        if( index==2) { //help page
        }

        qApp->processEvents();
        update();
    }
}

void QtRec::getOutVol( ) {
    Config cfg("qpe");
    cfg.setGroup("Volume");
    int outvol = cfg.readNumEntry("VolumePercent");
//   odebug << "out vol " << outvol << "" << oendl;
    OutputSlider->setValue( -outvol );
}

void QtRec::getInVol() {
    Config cfg("qpe");
    cfg.setGroup("Volume");
    int invol = cfg.readNumEntry("Mic");
//    odebug << "in vol " << invol << "" << oendl;
    InputSlider->setValue( -invol);
}

void QtRec::changedOutVolume() {
    int vol = -OutputSlider->value();

    Config cfg("qpe");
    cfg.setGroup("Volume");
    cfg.writeEntry("VolumePercent", QString::number( vol ));
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << false;

    owarn << "changing output vol " << vol << oendl;
}

void QtRec::changedInVolume( ) {
    int vol = -InputSlider->value();

    Config cfg("qpe");
    cfg.setGroup("Volume");
    cfg.writeEntry("Mic", QString::number( vol ));
    QCopEnvelope( "QPE/System", "micChange(bool)" ) << false;
}

bool QtRec::setupAudio( bool record ) {
    bool ok;
    snd_pcm_format_t sampleformat;
    int channels;

    if( !record ) {
        if( m_player.wavFile()->getResolution() == 16 )
            sampleformat = SND_PCM_FORMAT_S16;
        else
            sampleformat = SND_PCM_FORMAT_U8;

        channels = m_player.wavFile()->getChannels();
        filePara.sampleRate = m_player.wavFile()->getSampleRate();
    }
    else { // we want to record
        if( !bitRateComboBox->isEnabled() || bitRateComboBox->currentText() == "16")
            sampleformat = SND_PCM_FORMAT_S16;
        else
            sampleformat = SND_PCM_FORMAT_U8;

        if( stereoCheckBox->isChecked() )
            channels = 2;
        else
            channels = 1;

        filePara.sampleRate = sampleRateComboBox->currentText().toInt( &ok,10);//44100;
    }

    waveform->changeSettings( filePara.sampleRate, channels, sampleformat );

    if(autoMute)
        doMute(false);

    return true;
}

void QtRec::setupFileName() {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");

    QString fileName = m_dirPath;

    QDateTime dt = QDateTime::currentDateTime();
    QString date = dt.toString();
    date.replace(QRegExp("'"),"");
    date.replace(QRegExp(" "),"_");
    date.replace(QRegExp(":"),"-");
    date.replace(QRegExp(","),"");

    if(fileName.right(1).find("/",0,true) == -1)
        fileName += "/" + date;
    else
        fileName += date;
    fileName += ".wav";

    filePara.fileName = fileName;
}

/// <<<<<<<<<<<<<<<<  PLAY >>>>>>>>>>>>>>>>>>>
bool QtRec::doPlay() {
    if( mode != MODE_PAUSED ) {
        filePara.numberOfRecordedSeconds = 0;
    }
    else {
        secCount = (int)filePara.numberOfRecordedSeconds;
    }

    QString num;

    QString timeString;
    timeString.sprintf("%f",  filePara.numberOfRecordedSeconds);
//    timeLabel->setText( timeString+ tr(" seconds"));

    if(mode != MODE_PAUSED)
        startTimer( 1000);

    mode = MODE_PLAYING;

    setButtons();
    playIt();
    if(mode != MODE_PAUSED)
        endPlaying();
    return true;
}

void QtRec::changebitrateCombo(int i) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    int bits = 0;
    if( i == 1) { bits = 16; }
    else { bits=8; }
    cfg.writeEntry("bitrate", bits);
    cfg.write();
}

void QtRec::changesamplerateCombo(int i) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    int rate=0;
    bool ok;
    rate = sampleRateComboBox->text(i).toInt(&ok, 10);
    cfg.writeEntry( "samplerate",rate);
    cfg.write();
}

void QtRec::changeDirCombo(int index) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    QString sName = directoryComboBox->text(index);

    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString storage;
    for( ; it.current(); ++it ){
        if( sName == (*it)->name()+" "+  (*it)->path() ||
            (*it)->name() == sName ) {
            const QString path = (*it)->path();
            cfg.writeEntry("directory", path);
            odebug << "new rec dir " + path << oendl;
        }
    }
    cfg.write();
}

void QtRec::changeSizeLimitCombo(int) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    cfg.writeEntry("sizeLimit", getCurrentSizeLimit() );
    cfg.write();
}

void QtRec::newSound() {
    if(!rec()) {
        stop();
    }
}

void QtRec::deleteSound() {
    if( ListView1->currentItem() == NULL)
        return;

    if (QMessageBox::warning(this, tr("Delete"), tr("Are you sure?"), QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes) {
        QString file = getSelectedFile();
        QFile f( file );
        if( f.exists())
            if( !f.remove())
                QMessageBox::message( tr("Error"), tr("Could not remove file."));

        initIconView();
        update();
    }
}

void QtRec::keyPressEvent( QKeyEvent *e) {
    switch ( e->key() ) {
        case Key_Left: {
            if(mode == MODE_PLAYING && !e->isAutoRepeat()) {
                rewindPressed();
                e->accept();
            }
        }
            break;
        case Key_Right: {
            if(mode == MODE_PLAYING && !e->isAutoRepeat()) {
                FastforwardPressed();
                e->accept();
            }
        }
            break;
    }
}

void  QtRec::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
        case Key_Space:
            if(mode == MODE_RECORDING) {
                stop();
                e->accept();
            }
            else if (!renameBox) {
                doPlayBtn();
                e->accept();
            }
            break;
        case Key_Delete:
            if(deleteSoundButton->isEnabled()) {
                deleteSound();
                e->accept();
            }
            break;
        case Key_Left:
            if(mode == MODE_PAUSED && !e->isAutoRepeat())
                rewindReleased();
            break;
        case Key_Right:
            if(mode == MODE_PAUSED && !e->isAutoRepeat())
                FastforwardReleased();
            break;
    }
}

void QtRec::endRecording() {
    mode = MODE_IDLE;
    waveform->reset();

    setButtons();

    killTimers();

    if(autoMute)
        doMute( true);

    timeSlider->setValue(0);
    initIconView();
    odebug << QFileInfo(filePara.fileName).baseName() << oendl;
    selectItemByName( QFileInfo(filePara.fileName).fileName() );
}

void QtRec::endPlaying() {
    mode = MODE_IDLE;
    timeSlider->setValue(0);
    ListView1->setFocus();
    setButtons();

    waveform->reset();
    odebug << "end playing" << oendl;
    setButtons();

    if(autoMute)
        doMute( true);

//    timeLabel->setText("");

    killTimers();
}

QString QtRec::getSelectedFile() {
    QListViewItem *item = ListView1->currentItem();
    if(item)
        return m_dirPath + item->text(0);
    else
        return QString("");
}

bool QtRec::openPlayFile() {
    qApp->processEvents();
    QString fileName = getSelectedFile();
    if( fileName.isEmpty()) {
        QMessageBox::message(tr("Opierec"),tr("Please select file to play"));
        endPlaying();
        return false;
    }
    odebug << "opening file " << fileName << oendl;

    if( m_player.setup( fileName ) ) {
        timeSlider->setPageStep(1);
        timeSlider->setRange(0, m_player.wavFile()->getNumberSamples() );
    }
    else {
        // FIXME this is no longer sufficient, we need to get the actual error from the player object
        QString errorMsg = (QString)strerror(errno);
        QMessageBox::message(tr("Note"), tr("Could not open audio file.\n")
                             + errorMsg + "\n" + fileName);
        return false;
    }

    return true;
}

void QtRec::listPressed( int mouse, QListViewItem *item, const QPoint &, int ) {
    if(item == NULL )
        return;
    switch (mouse) {
        case 1: {
            if( renameBox != 0 ) //tricky
                cancelRename();

            setButtons();
        }
            break;
        case 2:
            if(mode == MODE_IDLE)
                showListMenu(item);
            break;
    };
}

void QtRec::showListMenu(QListViewItem * item) {
    if(item == NULL)
        return;
    QPopupMenu *m = new QPopupMenu(this);
    if(Ir::supported())  m->insertItem( tr( "Send with Ir" ), this, SLOT( doBeam() ));
    m->insertItem( tr( "Rename" ), this, SLOT( doRename() ));
    // #if defined (QTOPIA_INTERNAL_FSLP)
    //     m->insertItem( tr( "Properties" ), this, SLOT( doProperties() ));
    // #endif
    m->insertSeparator();
    m->insertItem( tr("Delete"), this, SLOT( deleteSound() ) );
    m->exec( QCursor::pos() );
    qApp->processEvents();
}

void QtRec::fileBeamFinished( Ir *ir) {
    if(ir)
        QMessageBox::message( tr("Ir Beam out"), tr("Ir sent.") ,tr("Ok") );

}

void QtRec::doBeam() {
    qApp->processEvents();
    if(  ListView1->currentItem() == NULL)
        return;
    Ir ir;
    if( ir.supported()) {
        QString file = ListView1->currentItem()->text(0);

//X        Ir *file = new Ir(this, "IR");
//X        connect( file, SIGNAL( done(Ir*)),
//X                    this, SLOT( fileBeamFinished(Ir*)));
//X        file->send( filePath, "OPieRec audio file\n" + filePath );
    }
}

void QtRec::doRename() {
    QRect r = ListView1->itemRect( ListView1->currentItem( ));
    r = QRect( ListView1->viewportToContents( r.topLeft() ), r.size() );
    r.setX( ListView1->contentsX() );
    if ( r.width() > ListView1->visibleWidth() )
        r.setWidth( ListView1->visibleWidth() );

    renameBox = new QLineEdit( ListView1->viewport(), "qt_renamebox" );
    renameBox->setFrame(true);

    QString filename = ListView1->currentItem()->text(0);
    filename = filename.left(filename.length() - 4); // remove extension
    renameBox->setText( filename );

    renameBox->selectAll();
    renameBox->installEventFilter( this );
    ListView1->addChild( renameBox, r.x(), r.y() );
    renameBox->resize( r.size() );
    ListView1->viewport()->setFocusProxy( renameBox );
    renameBox->setFocus();
    renameBox->show();

}

void QtRec::okRename() {
    QString newfilename = renameBox->text() + ".wav";
    cancelRename();

    if(  ListView1->currentItem() == NULL)
        return;

    QString oldfilename = ListView1->currentItem()->text(0);

    if(oldfilename == newfilename)
        return;

    odebug << "filename is  " + newfilename << oendl;

    FileManager fm;
    fm.renameFile(m_dirPath + oldfilename, m_dirPath + newfilename);

    initIconView();
    update();
}

void QtRec::cancelRename() {
    bool resetFocus = ListView1->viewport()->focusProxy() == renameBox;
    delete renameBox;
    renameBox = 0;
    if ( resetFocus ) {
        ListView1->viewport()->setFocusProxy( ListView1 );
        ListView1->setFocus();
    }
}

bool QtRec::eventFilter( QObject * o, QEvent * e ) {
    if ( o->inherits( "QLineEdit" ) ) {
        if ( e->type() == QEvent::KeyPress ) {
            QKeyEvent *ke = (QKeyEvent*)e;
            if ( ke->key() == Key_Return ||
                 ke->key() == Key_Enter ) {
                okRename();
                return true;
            } else if ( ke->key() == Key_Escape ) {
                cancelRename();
                return true;
            }
        } else if ( e->type() == QEvent::FocusOut ) {
            cancelRename();
            return true;
        }
    }
    return QWidget::eventFilter( o, e );
}

int QtRec::getCurrentSizeLimit() {
    return  sizeLimitCombo->currentItem() * 5;
}

void QtRec::doVolMuting(bool b) {
    Config cfg( "qpe" );
    cfg. setGroup( "Volume" );
    cfg.writeEntry( "Mute",b);
    cfg.write();
    QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << b;
}

void QtRec::doMicMuting(bool b) {
    //  odebug << "mic mute" << oendl;
    Config cfg( "qpe" );
    cfg. setGroup( "Volume" );
    cfg.writeEntry( "MicMute",b);
    cfg.write();
    QCopEnvelope( "QPE/System", "micChange(bool)" ) << b;
}

void QtRec::compressionSelected(bool b) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    cfg.writeEntry("wavCompression", b);
    cfg.writeEntry("bitrate", 16);
    cfg.write();

    if(b) {
        qWarning("set adpcm");
        bitRateComboBox->setCurrentItem( 1);
        bitRateComboBox->setEnabled( false);
    } else{
        bitRateComboBox->setEnabled( true);
    }
}

long QtRec::checkDiskSpace(const QString &path) {

    struct statfs fs;

    if ( !statfs( path.latin1(), &fs ) ) {

        int blkSize = fs.f_bsize;
        int availBlks = fs.f_bavail;

        long mult = blkSize / 1024;
        long div = 1024 / blkSize;

        if ( !mult ) mult = 1;
        if ( !div ) div = 1;

        return  availBlks * mult / div;
    }
    return -1;
}

//   short  f_fstyp;    /* File system type */
//   long  f_bsize;    /* Block size */
//   long  f_frsize;   /* Fragment size */
//   long  f_blocks;   /* Total number of blocks*/
//   long  f_bfree;    /* Count of free blocks */
//   long  f_files;    /* Total number of file nodes */
//   long  f_ffree;    /* Count of free file nodes */
//   char  f_fname[6]; /* Volumename */
//   char  f_fpack[6]; /* Pack name */

void QtRec::receive( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    if( msg == "toggleRecord()" )  {
        if( mode == MODE_RECORDING )
            stop();
        else if( mode == MODE_IDLE ) {
            if( isVisible() )
                newSound();
            else       // need to delay until window shown
                QTimer::singleShot(100, this, SLOT(newSound()));
        }
        QPEApplication::setKeepRunning();
    }
    else if( msg == "stop()" )  {
        if( mode != MODE_IDLE )
            stop();
    }
}

void QtRec::systemMsg( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    // Refresh the view e.g. if vmemo writes a new wave file
    if( msg == "linkChanged(QString)" )  {
        QString fileName;
        stream >> fileName;
        odebug << "QtRec linkChanged - " << fileName << oendl;
        if( fileName.find("audio/x-wav") > -1)
            if( m_dirPath.startsWith( QPEApplication::documentDir() ) )
                initIconView();
    }
}


///////////////////////////// timerEvent
void QtRec::timerEvent( QTimerEvent * ) {
    if( mode == MODE_RECORDING ) {
        timeSlider->setValue( secCount);
        if( secCount >= filePara.SecondsToRecord && filePara.SecondsToRecord != 0)
            stop();
    }

    odebug << "" << secCount << "" << oendl;
//    QString  timeString;
//    timeString.sprintf("%d", secCount);
//    timeLabel->setText( timeString + " seconds");

    secCount++;
}

void QtRec::changeTimeSlider(int index) {
    if( ListView1->currentItem() == 0 || mode != MODE_PLAYING ) return;
    //debug << "Slider moved to " << index << "" << oendl;    mode = MODE_PAUSED;

    sliderPos=index;

    QString timeString;
    filePara.numberOfRecordedSeconds = (float)sliderPos / (float)filePara.sampleRate * (float)2;
    timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
    secCount = (int)filePara.numberOfRecordedSeconds;
//    timeLabel->setText( timeString + tr(" seconds"));
}

void QtRec::timeSliderPressed() {
    if( ListView1->currentItem() == 0) return;
    odebug << "slider pressed" << oendl;
    mode = MODE_PAUSED;
}

void QtRec::timeSliderReleased() {
    if( ListView1->currentItem() == 0) return;
    sliderPos = timeSlider->value();

    odebug << "slider released " << sliderPos << "" << oendl;
    int newPos =  lseek( m_player.wavFile()->getfd(), sliderPos, SEEK_SET);
    filePara.numberOfRecordedSeconds = (float)sliderPos / (float)filePara.sampleRate * (float)2;

    if(mode == MODE_PAUSED)
        mode = MODE_PLAYING;
}

void QtRec::rewindPressed() {
    if( mode == MODE_PLAYING ) {
        killTimers();
        mode = MODE_PAUSED;
    }
    rewindTimer->start( 50, false);
}

void QtRec::rewindTimerTimeout() {
    int  sliderValue = timeSlider->value();
    sliderValue = sliderValue - ( filePara.sampleRate / 5 );
    timeSlider->setValue( sliderValue );
    filePara.numberOfRecordedSeconds = (float)sliderValue / (float)filePara.sampleRate * (float)2;
//    QString timeString;
//    timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
//    timeLabel->setText( timeString+ tr(" seconds"));
}

void QtRec::rewindReleased() {
    rewindTimer->stop();
    if( m_player.wavFile()->isOpen() ) {
        sliderPos = timeSlider->value();
        int newPos = lseek( m_player.wavFile()->getfd(), sliderPos, SEEK_SET);
        startTimer(1000);
        if(mode == MODE_PAUSED)
            mode = MODE_PLAYING;
    }
}

void QtRec::FastforwardPressed() {
    if( mode == MODE_PLAYING ) {
        killTimers();
        mode = MODE_PAUSED;
    }
    forwardTimer->start(50, false);
}

void QtRec::forwardTimerTimeout() {
    int sliderValue = timeSlider->value();
    sliderValue = sliderValue + ( filePara.sampleRate / 5 );
    timeSlider->setValue( sliderValue );
    filePara.numberOfRecordedSeconds = (float)sliderValue / (float)filePara.sampleRate * (float)2;
//    QString timeString;
//    timeString.sprintf( "%.2f", filePara.numberOfRecordedSeconds);
//    timeLabel->setText( timeString+ tr(" seconds"));
}

void QtRec::FastforwardReleased() {
    forwardTimer->stop();
    if( m_player.wavFile()->isOpen()) {
        sliderPos = timeSlider->value();
        int newPos = lseek( m_player.wavFile()->getfd(), sliderPos, SEEK_SET);
        startTimer(1000);
        if(mode == MODE_PAUSED)
            mode = MODE_PLAYING;
    }
}


QString QtRec::getStorage(const QString &fileName) {

    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString storage;
    for( ; it.current(); ++it ){
        const QString name = ( *it)->name();
        const QString path = ( *it)->path();
        const QString disk = ( *it)->disk();
        if( fileName.find( path,0,true) != -1)
            storage = name;
        //        const QString options = (*it)->options();
        //        if( name.find( tr("Internal"),0,true) == -1) {
        //            storageComboBox->insertItem( name +" -> "+disk);
        //    odebug << name << oendl;
    }
    return storage;
    //      struct mntent *me;
    //  //     if(fileName == "/etc/mtab") {
    //      FILE *mntfp = setmntent( fileName.latin1(), "r" );
    //      if ( mntfp ) {
    //          while ( (me = getmntent( mntfp )) != 0 ) {
    //              QString filesystemType = me->mnt_type;

    //          }
    //      }
    //      endmntent( mntfp );
}

void QtRec::fillDirectoryCombo() {
    if( directoryComboBox->count() > 0)
        directoryComboBox->clear();
    int index = 0;
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    QString dir = cfg.readEntry("directory", QPEApplication::documentDir());
    StorageInfo storageInfo;
    const QList<FileSystem> &fs = storageInfo.fileSystems();
    QListIterator<FileSystem> it ( fs );
    QString storage;
    for( ; it.current(); ++it ){
        const QString name = ( *it)->name();
        const QString path = ( *it)->path();
        //    directoryComboBox->insertItem(name+" "+path);
        directoryComboBox->insertItem(name);
        if( path == dir)
            directoryComboBox->setCurrentItem( index);
        index++;
    }
}

void QtRec::errorStop() {
    mode = MODE_IDLE;
    killTimers();
}

void QtRec::doMute(bool b) {
    doVolMuting( b);
    doMicMuting( b);
}

void QtRec::slotAutoMute(bool b) {
    autoMute = b;
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    cfg.writeEntry("useAutoMute",b);
    doMute( b);
    outMuteCheckBox->setChecked( b);
    inMuteCheckBox->setChecked( b);
}

void QtRec::selectItemByName(const QString & name) {
    QListViewItemIterator it( ListView1 );
    for ( ; it.current(); ++it ) {
        if( name == it.current()->text(0)) {
            ListView1->setCurrentItem(it.current());
            setButtons();
            break;
        }
    }
}


// long findPeak(long input ) {

// // halfLife = time in seconds for output to decay to half value after an impulse
//    static float output = 0.0;
//    float halfLife = .0025;
//    float vsf = .0025;
//    float scalar = pow( 0.5, 1.0/(halfLife * filePara.sampleRate ));
//    if( input < 0.0 )
//       input = -input;  // Absolute value.
//    if ( input >= output ) {
//       // When we hit a peak, ride the peak to the top.
//       output = input;
//    } else {
// // Exponential decay of output when signal is low.
//       output = output * scalar;
// //
// //   When current gets close to 0.0, set current to 0.0 to prevent FP underflow
// //   which can cause a severe performance degradation due to a flood
// //   of interrupts.
// //
//       if( output < vsf ) output = 0.0;
//    }

//    return (long) output;
// }

void QtRec::changeStereoCheck(bool b) {
    Config cfg("OpieRec");
    cfg.setGroup("Settings");
    cfg.writeEntry("stereo", b);
    cfg.write();
}

void QtRec::setButtons() {
    if( mode == MODE_IDLE ) {
        Stop_PushButton->setPixmap( Opie::Core::OResource::loadPixmap("play2", Opie::Core::OResource::SmallIcon) );
        if(ListView1->currentItem()) {
            Stop_PushButton->setEnabled(true);
            deleteSoundButton->setEnabled(true);
        }
        else {
            Stop_PushButton->setEnabled(false);
            deleteSoundButton->setEnabled(false);
        }
        Rec_PushButton->setEnabled(true);
        toBeginningButton->setEnabled(false);
        toEndButton->setEnabled(false);
        timeSlider->setEnabled(false);
    }
    else {
        timeSlider->setEnabled( mode != MODE_RECORDING );
        Stop_PushButton->setPixmap( Opie::Core::OResource::loadPixmap("stop", Opie::Core::OResource::SmallIcon) );
        Stop_PushButton->setEnabled(true);
        toBeginningButton->setEnabled( mode == MODE_PLAYING );
        toEndButton->setEnabled( mode == MODE_PLAYING );
        deleteSoundButton->setEnabled(false);
        Rec_PushButton->setEnabled(false);
    }
}

void QtRec::fileSize(unsigned long size, QString &str) {
    if( size > 1048576 )
        str.sprintf( "%.0fM", size / 1048576.0 );
    else if( size > 1024 )
        str.sprintf( "%.0fk", size / 1024.0 );
    else
        str.sprintf( "%ld", size );
}
