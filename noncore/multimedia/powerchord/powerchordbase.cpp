/****************************************************************************
** Form implementation generated from reading ui file 'powerchordbase.ui'
**
** Created: Sun Jan 13 23:05:11 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "powerchordbase.h"
#include "fretboard.h"
#include "vumeter.h"

/* OPIE */
#include <opie2/odebug.h>
#include <qpe/resource.h>
using namespace Opie::Core;

/* QT */
#include <qcombobox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qtooltip.h>

/*
 *  Constructs a PowerchordBase which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
PowerchordBase::PowerchordBase( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
  simulation_timer = 0;
  audio_timer = 0;

  //    setPalette( QPalette( QColor( 232, 227, 215) ) );

  //  QPixmap image0(QString("/opt/Qtopia/pics/powerchord/image0"));
  QPixmap image1 = Resource::loadPixmap( "powerchord/image1");
  QPixmap image2 = Resource::loadPixmap( "powerchord/image2");
  QPixmap image3 = Resource::loadPixmap( "powerchord/image3");
  QPixmap image4 = Resource::loadPixmap( "powerchord/image4");
  QPixmap image5 = Resource::loadPixmap( "powerchord/image5");
  image6 = Resource::loadPixmap( "powerchord/image6");
  image_open = Resource::loadPixmap( "powerchord/image_open");

  //  image0.setMask(image0.createHeuristicMask());
   image1.setMask(image1.createHeuristicMask());
    //    image2.setMask(image2.createHeuristicMask());
    //    image3.setMask(image3.createHeuristicMask());
    //    image4.setMask(image4.createHeuristicMask());
    //    image5.setMask(image5.createHeuristicMask());
    //    image6->setMask(image6->createHeuristicMask());
    //    image_open->setMask(image_open->createHeuristicMask());

    if ( !name )
	setName( "PowerchordBase" );
    resize( 240, 284 );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 240, 284 ) );
    setMaximumSize( QSize( 240, 284 ) );
    setCaption( tr( "Powerchord" ) );

    tabs = new QTabWidget( this, "tabs" );
    tabs->setEnabled( TRUE );
    tabs->setGeometry( QRect( 0, 0, 240, 286 ) ); 
    tabs->setTabPosition( QTabWidget::Bottom );

    tab = new QWidget( tabs, "tab" );

    chordkey = new QComboBox( FALSE, tab, "chordkey" );
    chordkey->insertItem( tr( "maj" ) );
    chordkey->insertItem( tr( "min" ) );
    chordkey->insertItem( tr( "7th" ) );
    chordkey->insertItem( tr( "m7" ) );
    chordkey->insertItem( tr( "maj7" ) );
    chordkey->insertItem( tr( "6th" ) );
    chordkey->insertItem( tr( "m6th" ) );
    chordkey->insertItem( tr( "aug" ) );
    chordkey->insertItem( tr( "dim" ) );
    chordkey->insertItem( tr( "sus4" ) );
    chordkey->insertItem( tr( "7sus4" ) );
    chordkey->insertItem( tr( "9th" ) );
    chordkey->insertItem( tr( "add9" ) );
    chordkey->insertItem( tr( "m9th" ) );
    chordkey->insertItem( tr( "maj9" ) );
    chordkey->insertItem( tr( "sus2" ) );
    chordkey->insertItem( tr( "7sus2" ) );
    chordkey->insertItem( tr( "11th" ) );
    chordkey->insertItem( tr( "m11th" ) );
    chordkey->insertItem( tr( "13th" ) );
    chordkey->insertItem( tr( "m13th" ) );
    chordkey->insertItem( tr( "maj13" ) );
    chordkey->insertItem( tr( "6/9" ) );
    chordkey->insertItem( tr( "flat5" ) );
    chordkey->insertItem( tr( "7#9" ) );
    chordkey->insertItem( tr( QString::fromUtf8( "ø7" ) ) );
    chordkey->insertItem( tr( "5" ) );
    chordkey->setGeometry( QRect( 40, 0, 51, 21 ) ); 

    chordfret = new QComboBox( FALSE, tab, "chordfret" );
    chordfret->insertItem( tr( "open" ) );
    chordfret->insertItem( tr( "1st" ) );
    chordfret->insertItem( tr( "2nd" ) );
    chordfret->insertItem( tr( "3rd" ) );
    chordfret->insertItem( tr( "4th" ) );
    chordfret->insertItem( tr( "5th" ) );
    chordfret->insertItem( tr( "6th" ) );
    chordfret->insertItem( tr( "7th" ) );
    chordfret->insertItem( tr( "8th" ) );
    chordfret->insertItem( tr( "9th" ) );
    chordfret->insertItem( tr( "10th" ) );
    chordfret->insertItem( tr( "11th" ) );
    chordfret->insertItem( tr( "12th" ) );
    chordfret->insertItem( tr( "13th" ) );
    chordfret->insertItem( tr( "14th" ) );
    chordfret->insertItem( tr( "15th" ) );
    chordfret->insertItem( tr( "16th" ) );
    chordfret->setGeometry( QRect( 90, 0, 55, 21 ) ); //cxm less 5 width

    chordnote = new QComboBox( FALSE, tab, "chordnote" );
    chordnote->insertItem( tr( "C" ) );
    chordnote->insertItem( tr( "C#" ) );
    chordnote->insertItem( tr( "D" ) );
    chordnote->insertItem( tr( "Eb" ) );
    chordnote->insertItem( tr( "E" ) );
    chordnote->insertItem( tr( "F" ) );
    chordnote->insertItem( tr( "F#" ) );
    chordnote->insertItem( tr( "G" ) );
    chordnote->insertItem( tr( "G#" ) );
    chordnote->insertItem( tr( "A" ) );
    chordnote->insertItem( tr( "Bb" ) );
    chordnote->insertItem( tr( "B" ) );
    chordnote->setGeometry( QRect( 0, 0, 40, 21 ) ); 
    chordnote->setCurrentItem( 9 );

    QWidget* privateLayoutWidget = new QWidget( tab, "Layout1" );
    privateLayoutWidget->setGeometry( QRect( 5, 232, 160, 20 ) ); 
    Layout1 = new QHBoxLayout( privateLayoutWidget ); 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    s1_1 = new QLabel( privateLayoutWidget, "s1_1" );
    s1_1->setText( tr( "E" ) );
    s1_1->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_1 );

    s1_2 = new QLabel( privateLayoutWidget, "s1_2" );
    s1_2->setText( tr( "A" ) );
    s1_2->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_2 );

    s1_3 = new QLabel( privateLayoutWidget, "s1_3" );
    s1_3->setText( tr( "E" ) );
    s1_3->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_3 );

    s1_4 = new QLabel( privateLayoutWidget, "s1_4" );
    s1_4->setText( tr( "A" ) );
    s1_4->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_4 );

    s1_5 = new QLabel( privateLayoutWidget, "s1_5" );
    s1_5->setText( tr( "C#" ) );
    s1_5->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_5 );

    s1_6 = new QLabel( privateLayoutWidget, "s1_6" );
    s1_6->setText( tr( "E" ) );
    s1_6->setAlignment( int( QLabel::AlignCenter ) );
    Layout1->addWidget( s1_6 );

    //    sound_label = new QLabel( tab, "sound_label" );
    //    sound_label->setGeometry( QRect( 185, 160, 32, 17 ) ); 
    //    sound_label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, sound_label->sizePolicy().hasHeightForWidth() ) );
    //    sound_label->setPixmap( image0 );
    //    sound_label->pixmap()->setMask(*image0.mask());
    //    sound_label->setScaledContents( TRUE );
    //  sound_label->setBackgroundMode( QWidget::PaletteButton );
    //    sound_label->setBackgroundColor(this->backgroundColor());
    //    sound_label->setPalette( QPalette( QColor( 232, 227, 215) ) );


    Frame6 = new QFrame( tab, "Frame6" );
    Frame6->setGeometry( QRect( 170, 145, 66, 10 ) ); 
    Frame6->setFrameShape( QFrame::HLine );
    Frame6->setFrameShadow( QFrame::Raised );

    chordshow_label = new QLabel( tab, "chordshow_label" );
    chordshow_label->setGeometry( QRect( 185, 60, 32, 17 ) ); 
    //    chordshow_label->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, chordshow_label->sizePolicy().hasHeightForWidth() ) );
    chordshow_label->setFrameShape( QLabel::NoFrame );
    chordshow_label->setPixmap( image1 );
    chordshow_label->setScaledContents( TRUE );
    //    chordshow_label->setBackgroundMode( QWidget::PaletteButton );

    transport_rec = new QPushButton( tab, "transport_rec" );
    transport_rec->setGeometry( QRect( 170, 80, 30, 30 ) ); 
    transport_rec->setText( "" );
    transport_rec->setPixmap( image2 );

    play_sound = new QPushButton( tab, "play_sound" );
    play_sound->setGeometry( QRect( 170, 180, 60, 55 ) ); 
    play_sound->setText( "" );
    play_sound->setPixmap( image3 );

    Frame6_2 = new QFrame( tab, "Frame6_2" );
    Frame6_2->setGeometry( QRect( 170, 45, 66, 10 ) ); 
    Frame6_2->setFrameShape( QFrame::HLine );
    Frame6_2->setFrameShadow( QFrame::Raised );

    transport_play = new QPushButton( tab, "transport_play" );
    transport_play->setEnabled( FALSE );
    transport_play->setGeometry( QRect( 200, 80, 30, 30 ) ); 
    transport_play->setText( "" );
    transport_play->setPixmap( image3 );

    transport_rew = new QPushButton( tab, "transport_rew" );
    transport_rew->setEnabled( FALSE );
    transport_rew->setGeometry( QRect( 170, 110, 30, 30 ) ); 
    transport_rew->setText( "" );
    transport_rew->setPixmap( image4 );

    transport_fwd = new QPushButton( tab, "transport_fwd" );
    transport_fwd->setEnabled( FALSE );
    transport_fwd->setGeometry( QRect( 200, 110, 30, 30 ) ); 
    transport_fwd->setText( "" );
    transport_fwd->setPixmap( image5 );

    chordname = new QLabel( tab, "chordname" );
    chordname->setGeometry( QRect( 146, 0, 90, 20 ) ); //cxm moved l 5
    chordname->setText( tr( "A" ) );
    chordname->setAlignment( int( QLabel::AlignCenter ) );
    QToolTip::add(  chordname, "" );

    synth = new gs();
    tuner = new gt();
    frets = new FretBoard( tab, "frets" );
    frets->setGeometry( QRect( 0, 20, 168, 210 ) ); 
    tabs->insertTab( tab, tr( "Guitar" ) );

    tab_2 = new QWidget( tabs, "tab_2" );

    optlab2 = new QLabel( tab_2, "optlab2" );
    optlab2->setGeometry( QRect( 9, 58, 120, 41 ) ); 
    optlab2->setText( tr( "Alternative tunings are possible" ) );
    optlab2->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );

    span = new QComboBox( FALSE, tab_2, "span" );
    span->insertItem( tr( "2" ) );
    span->insertItem( tr( "3" ) );
    span->insertItem( tr( "4" ) );
    span->insertItem( tr( "5" ) );
    span->insertItem( tr( "6" ) );
    span->setGeometry( QRect( 180, 20, 50, 21 ) ); 
    span->setCurrentItem( 1 );

    TextLabel1 = new QLabel( tab_2, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 3, 109, 226, 140 ) ); 
    TextLabel1->setText( tr( QString::fromUtf8( "©2002 Camilo Mesias\n"
"camilo@mesias.co.uk\n"
"Version 0.0.7 beta" ) ) );
    TextLabel1->setAlignment( int( QLabel::AlignBottom | QLabel::AlignLeft ) );

    tuning = new QComboBox( FALSE, tab_2, "tuning" );
    tuning->insertItem( tr( "EADGBE" ) );
    tuning->insertItem( tr( "EBEG#BE" ) );
    tuning->insertItem( tr( "EAEAC#E" ) );
    tuning->insertItem( tr( "EADF#BE" ) );
    tuning->insertItem( tr( "EADGCF" ) );
    tuning->insertItem( tr( "DADGAD" ) );
    tuning->insertItem( tr( "DGCGCD" ) );
    tuning->insertItem( tr( "DADF#AD" ) );
    tuning->insertItem( tr( "DADGBE" ) );
    tuning->insertItem( tr( "DGDGBD" ) );
    tuning->insertItem( tr( "DADACD" ) );
    tuning->insertItem( tr( "CGCGAE" ) );
    tuning->insertItem( tr( "FADGBE" ) );
    tuning->insertItem( tr( "G minor" ) );
    tuning->setGeometry( QRect( 140, 70, 91, 21 ) ); 

    optlab1 = new QLabel( tab_2, "optlab1" );
    optlab1->setGeometry( QRect( 8, 8, 160, 40 ) ); 
    optlab1->setText( tr( "Span: the greatest number of frets over which you want chords to be generated" ) );
    optlab1->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignLeft ) );
    tabs->insertTab( tab_2, tr( "Options" ) );

    Tuner = new QWidget( tabs, "Tuner" );

    Frame4 = new QFrame( Tuner, "Frame4" );
    Frame4->setGeometry( QRect( 0, 0, 230, 126 ) ); 
    Frame4->setFrameShape( QFrame::Box );
    Frame4->setFrameShadow( QFrame::Raised );

    vu = new VUMeter( Frame4, "vu" );
    vu->setGeometry( QRect( 5, 5, 220, 115 ) ); 

    tuner_note = new QComboBox( FALSE, Tuner, "tuner_note" );
    tuner_note->insertItem( tr( "auto" ) );
    tuner_note->insertItem( tr( "E" ) );
    tuner_note->insertItem( tr( "D" ) );
    tuner_note->insertItem( tr( "G" ) );
    tuner_note->insertItem( tr( "A" ) );
    tuner_note->insertItem( tr( "B" ) );
    tuner_note->insertItem( tr( "C" ) );
    tuner_note->insertItem( tr( "F" ) );
    tuner_note->setGeometry( QRect( 5, 160, 90, 20 ) ); 

    Frame8 = new QFrame( Tuner, "Frame8" );
    Frame8->setGeometry( QRect( -2, 188, 231, 10 ) ); 
    Frame8->setFrameShape( QFrame::HLine );
    Frame8->setFrameShadow( QFrame::Raised );

    tuner_start = new QPushButton( Tuner, "tuner_start" );
    tuner_start->setGeometry( QRect( 100, 155, 91, 30 ) ); 
    tuner_start->setText( tr( "Start" ) );

    tuner_lab1 = new QLabel( Tuner, "tuner_lab1" );
    tuner_lab1->setGeometry( QRect( 10, 130, 100, 21 ) ); 
    tuner_lab1->setText( tr( "Tuner operation" ) );

    tuner_lab2 = new QLabel( Tuner, "tuner_lab2" );
    tuner_lab2->setGeometry( QRect( 10, 200, 100, 16 ) ); 
    tuner_lab2->setText( tr( "Pitch calibration" ) );

    tuner_calib_note = new QComboBox( FALSE, Tuner, "tuner_calib_note" );
    tuner_calib_note->insertItem( tr( "A" ) );
    tuner_calib_note->insertItem( tr( "C" ) );
    tuner_calib_note->setGeometry( QRect( 5, 225, 90, 21 ) ); 

    tuner_calib_freq = new QSpinBox( Tuner, "tuner_calib_freq" );
    tuner_calib_freq->setGeometry( QRect( 105, 225, 71, 21 ) ); 
    tuner_calib_freq->setSuffix( tr( "Hz" ) );
    tuner_calib_freq->setWrapping( TRUE );
    tuner_calib_freq->setMaxValue( 500 );
    tuner_calib_freq->setMinValue( 300 );
    tuner_calib_freq->setValue( 440 );

    tuner_lab3 = new QLabel( Tuner, "tuner_lab3" );
    tuner_lab3->setGeometry( QRect( 185, 225, 40, 21 ) ); 
    tuner_lab3->setText( tr( "Pitch" ) );

    tuner_pic1 = new QLabel( Tuner, "tuner_pic1" );
    tuner_pic1->setGeometry( QRect( 195, 155, 31, 31 ) ); 
    tuner_pic1->setFrameShape( QLabel::NoFrame );
    tuner_pic1->setPixmap( image6 );
    tuner_pic1->setScaledContents( TRUE );
    tabs->insertTab( Tuner, tr( "Tuner" ) );

    tab_3 = new QWidget( tabs, "tab_3" );

    chordlistlab1 = new QLabel( tab_3, "chordlistlab1" );
    chordlistlab1->setGeometry( QRect( 5, 5, 216, 16 ) );
    chordlistlab1->setText( tr( "Chord list" ) );

    chordlist = new QListBox( tab_3, "chordlist" );
    chordlist->setGeometry( QRect( 5, 25, 220, 200 ) );

    list_remove_btn = new QPushButton( tab_3, "list_remove_btn" );
    list_remove_btn->setGeometry( QRect( 150, 230, 71, 25 ) );
    list_remove_btn->setText( tr( "Remove" ) );
    tabs->insertTab( tab_3, tr( "Chords" ) );

    // hope this does what it's supposed to!!
    //    setPalette( QPalette( QColor( 232, 227, 215) ) );
    
    // signals and slots connections

    //cxm
    connect( tuning, SIGNAL( activated(int) ), frets, SLOT( Tune(int) ) );
    connect( span, SIGNAL( activated(int) ), frets, SLOT( Span(int) ) );
    connect( transport_rec, SIGNAL( clicked() ), this, SLOT( transport_rec_cb() ));
    connect( play_sound, SIGNAL( clicked() ), this, SLOT( play_chord_cb() ));
    connect( tuner_start, SIGNAL( clicked() ), this, SLOT( tuner_start_cb() ));
    connect( tuner_note, SIGNAL( textChanged(const QString&) ), vu, SLOT( AnnotL(const QString&) ));
    connect( this, SIGNAL( frequency_change(int) ), vu, SLOT( Value(int) ));

    connect( chordnote, SIGNAL( activated(int) ), frets, SLOT( Base(int) ) );
    connect( chordkey, SIGNAL( activated(int) ), frets, SLOT( Chord(int) ) );
    connect( chordfret, SIGNAL( activated(int) ), frets, SLOT( Fret(int) ) );
    connect( list_remove_btn, SIGNAL( clicked() ), this, SLOT( list_remove_cb() ) );
    connect( frets, SIGNAL( s1nameChanged(const QString&) ), s1_1, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( s2nameChanged(const QString&) ), s1_2, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( s3nameChanged(const QString&) ), s1_3, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( s4nameChanged(const QString&) ), s1_4, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( s5nameChanged(const QString&) ), s1_5, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( s6nameChanged(const QString&) ), s1_6, SLOT( setText(const QString&) ) );
    connect( frets, SIGNAL( nameChanged(const QString&) ), chordname, SLOT( setText(const QString&) ) );
}


static int known=0;
#include <qmessagebox.h>
void PowerchordBase::transport_rec_cb(){
  chordlist->insertItem(chordname->text(),-1);
  if (!known){
    QMessageBox::information(this, tr("Powerchord"),
			 tr("<P>This chord has been saved into the list of chords, for later playback.<P>"));
    known = 1;
  }
}
void PowerchordBase::list_remove_cb(){
  if (chordlist->count() > 0){
    chordlist->removeItem(0);
  }
}
void PowerchordBase::play_chord_cb(){
  //    QMessageBox::information(this, "Coming soon!", "This button plays\nthe chord by synthesizing\nthe sound of the notes.");


    if (audio_timer){
      audio_timer->stop();
      // set pixmap?
      synth->Stop();
      delete(audio_timer);
      audio_timer = 0;
    }else{
      // get notes from chord engine
      
      int note;
      int base;
      int octave;
      note = frets->ce.noteindex(0);
      if (note >= 0){
	//	fprintf(stderr, "Note was %d\n", note);
	base = note % 12;
        octave = note / 12;
	synth->note_start(0, base, octave);
      }else{
	// subtle bug here - replay second note if 1st one muted
	note = frets->ce.noteindex(1);
	base = note % 12;
        octave = note / 12;
	synth->note_start(1, base, octave);

      }

      // init synth
      if (synth->Play()){
	// error
	QMessageBox::information(this, tr("Powerchord"),
			 tr("<P>Unable to open device for sound playback - check that no other application is using it.</P>"));
	return;
      }
      synth->fill_buffer();
      
      // start timer
      audio_timer = new QTimer();
      connect(audio_timer, SIGNAL( timeout() ), this, SLOT( audio_cb() ));
      // set pixmap on player?
      audio_timer->start(19); // 19 msec (fudge factor!!)
    }
}



void PowerchordBase::audio_cb(){
  // play the next bit of audio until quiet

  // strum timing
#define INTERVAL 2

  if ((synth->Frames() % INTERVAL) == 0){
    int string = synth->Frames() / INTERVAL;
    if (string <= 5){
      int note;
      int base;
      int octave;
      note = frets->ce.noteindex(string);
      // check not muted...
      if (note > 0){
	base = note % 12;
	octave = note / 12;
	synth->note_start(string, base, octave);
      }
    }
  }
  
  if (synth->Playing()){
    synth->write_buffer();
    synth->fill_buffer();
  }else{
    audio_timer->stop();
    // set pixmap?
    synth->Stop();
    delete(audio_timer);
    audio_timer = 0;
  }
}

// the real tuner
//  void PowerchordBase::tuner_start_cb(){

//    if (tuner->Listening()){
//      simulation_timer->stop();
//      tuner->Stop();
//      tuner_pic1->setPixmap( *image6 );
    
//    }else{
//      tuner_pic1->setPixmap( *image_open );
    
//      if (!simulation_timer){
//        simulation_timer = new QTimer();
      
      
//        connect(simulation_timer, SIGNAL( timeout() ), this, SLOT( tuner_cb() ));
//      }
    
//      simulation_timer->start(50);
//      tuner->Listen();
    
//    }  
//  }

void PowerchordBase::tuner_cb(){
  if (tuner->Listening()){
    tuner->read_buffer();
    tuner->process_buffer();
    
    // update gui
    // do something with the note: tuner->Note()
    emit frequency_change(tuner->Tuning());
    
  }else{
    simulation_timer->stop();
    tuner->Stop();
    tuner_pic1->setPixmap( image6 );
  }
}


void PowerchordBase::tuner_start_cb(){
  if (0 == QMessageBox::information(this, tr("Powerchord"),
					tr("<P>Using the microphone, the note's frequency is analysed. This is a simulation.</P>"),
					tr("OK"), tr("Cancel"), 0, 1)){
    if (simulation_timer){
      simulation_timer->stop();
    }else{
      simulation_timer = new QTimer();
      connect(simulation_timer, SIGNAL( timeout() ), this, SLOT( tuner_simulation_cb() ));
      
    }
    simulation_x = -45;
    simulation_v = 0;
    simulation_iter = 0;

    tuner_pic1->setPixmap( image_open );
    simulation_timer->start(100);
  }
}

#include <stdio.h>

void PowerchordBase::tuner_simulation_cb(){
  if (simulation_x < -10 || simulation_x > 10){
    simulation_v = (simulation_v/2)-(simulation_x/5);
  }
  
  simulation_x += simulation_v;
  simulation_iter++;

  if (simulation_x > 50){
    simulation_x = 50;
  }

  if (simulation_x < -50){
    simulation_x = -50;
  }

  if (simulation_iter > 50){
    simulation_timer->stop();
    emit frequency_change(0);
    tuner_pic1->setPixmap( image6 );
  }else{
    emit frequency_change(simulation_x);
  }
}

/*
 *  Destroys the object and frees any allocated resources
 */
PowerchordBase::~PowerchordBase()
{
    // no need to delete child widgets, Qt does it all for us
}

void PowerchordBase::change_handler()
{
    owarn << "PowerchordBase::change_handler(): Not implemented yet!" << oendl; 
}

