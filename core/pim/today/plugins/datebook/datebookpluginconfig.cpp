

#include "datebookpluginconfig.h"

#include <qpe/config.h>

#include <qlayout.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qwhatsthis.h>
#include <qvbox.h>

DatebookPluginConfig::DatebookPluginConfig( QWidget* parent, const char* name)
    : TodayConfigWidget( parent, name ) {

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->setMargin( 20 );

    QHBox *box1 = new QHBox( this );
    QLabel* TextLabel4 = new QLabel( box1, "TextLabel4" );
    TextLabel4->setText( tr( "Show location" ) );
    CheckBox1 = new QCheckBox( box1, "CheckBox1" );
    QWhatsThis::add( CheckBox1 , tr( "Check this if the location of an appointment should be shown for each one" ) );

    QHBox *box2 = new QHBox( this );
    QLabel* TextLabel5 = new QLabel( box2 , "TextLabel5" );
    TextLabel5->setText( tr( "Show notes" ) );
    CheckBox2 = new QCheckBox( box2, "CheckBox2" );
    QWhatsThis::add( CheckBox2 , tr( "Check this if the note attached to an appointment should be shown for each one" ) );

    QHBox *box3 = new QHBox( this );
    QLabel* TextLabel6 = new QLabel( box3, "All Day");
    TextLabel6->setText( tr( "Show only later\n appointments") );
    CheckBox3 = new QCheckBox ( box3, "CheckBox3" );
    QWhatsThis::add( CheckBox3 , tr( "Check this if only appointments later then current time should be shown" ) );

    QHBox *box4 = new QHBox( this );
    QLabel *TextLabel3 = new QLabel( box4, "TextLabel3" );
    TextLabel3->setText( tr( "How many \nappointment\n"
                             "should be \nshown?" ) );
    SpinBox1 = new QSpinBox( box4, "SpinBox1" );
    QWhatsThis::add( SpinBox1 , tr( "How many appointments should be shown maximal. In chronical order" ) );
    SpinBox1->setMaxValue( 10 );
    SpinBox1->setValue( 5 );

    layout->addWidget( box1 );
    layout->addWidget( box2 );
    layout->addWidget( box3 );
    layout->addWidget( box4 );

    readConfig();
}

void DatebookPluginConfig::readConfig() {
    Config cfg( "todaydatebookplugin" );
    cfg.setGroup( "config" );

    m_max_lines_meet = cfg.readNumEntry( "maxlinesmeet", 5 );
    SpinBox1->setValue( m_max_lines_meet );
    m_show_location = cfg.readNumEntry( "showlocation", 1 );
    CheckBox1->setChecked( m_show_location );
    m_show_notes = cfg.readNumEntry( "shownotes", 0 );
    CheckBox2->setChecked( m_show_notes );
    m_only_later = cfg.readNumEntry( "onlylater", 1 );
    CheckBox3->setChecked( m_only_later );
}


void DatebookPluginConfig::writeConfig() {
    Config cfg( "todaydatebookplugin" );
    cfg.setGroup( "config" );

    m_max_lines_meet = SpinBox1->value();
    cfg.writeEntry( "maxlinesmeet",  m_max_lines_meet);
    m_show_location = CheckBox1->isChecked();
    cfg.writeEntry( "showlocation", m_show_location);
    m_show_notes = CheckBox2->isChecked();
    cfg.writeEntry( "shownotes", m_show_notes );
     m_only_later  = CheckBox3->isChecked();
    cfg.writeEntry( "onlylater", m_only_later );
    cfg.write();
}

DatebookPluginConfig::~DatebookPluginConfig() {
}
