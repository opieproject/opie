#include <qbuttongroup.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qvbox.h>
#include <qhgroupbox.h>
#include <qhbuttongroup.h>
#include <qlayout.h>
#include <qhbox.h>

#include "terminalwidget.h"

namespace {
    enum TermIds {
        id_term_vt100 = 0,
        id_term_vt102,
        id_term_linux,
        id_term_xterm
    };

    enum ColourIds {
        id_term_black,
        id_term_white,
        id_term_green,
        id_term_orange
    };

    enum FontIds {
        id_size_small,
        id_size_medium,
        id_size_large
    };
};

TerminalWidget::TerminalWidget( const QString& name, QWidget* parent,
                                const char* na )
    : ProfileDialogTerminalWidget( name, parent, na ) {

    m_terminal = new QLabel(tr("Terminal Type"), this );
    m_terminalBox = new QComboBox(this);
    m_colorLabel = new QLabel(tr("Color scheme"), this);
    m_colorCmb = new QComboBox(this );

    m_groupSize = new QHButtonGroup(tr("Font size"), this );
    m_sizeSmall = new QRadioButton(tr("small"),  m_groupSize );
    m_sizeMedium = new QRadioButton(tr("medium"),  m_groupSize );
    m_sizeLarge = new QRadioButton(tr("large"),  m_groupSize );

    m_groupConv = new QHGroupBox(tr("Line-break conversions"), this );
    m_convInbound  = new QCheckBox(tr("Inbound"), m_groupConv );
    m_convOutbound = new QCheckBox(tr("Outbound"), m_groupConv );
    m_groupConv->hide();

    m_groupOptions = new QHGroupBox( tr("Options"), this );
    m_optionEcho = new QCheckBox(tr("Local echo"), m_groupOptions );
    m_optionWrap = new QCheckBox(tr("Line wrap"), m_groupOptions );
    m_groupOptions->hide();

    m_lroot = new QVBoxLayout( this  );
    m_typeBox = new  QVBoxLayout( m_lroot );
    m_colorBox = new QVBoxLayout( m_lroot );

    // Layout
    m_typeBox->add(m_terminal );
    m_typeBox->add(m_terminalBox );
    m_lroot->add(m_groupSize );

    m_colorBox->add(  m_colorLabel );
    m_colorBox->add(  m_colorCmb   );

    m_lroot->add(m_groupConv );
    m_lroot->add(m_groupOptions );
    m_lroot->addStretch( 0 );

    // Fill in some options
    qWarning("Options for terminal box");
    m_terminalBox->insertItem( tr("VT 100"), 0 ); // /*, id_term_vt100*/ );
    m_terminalBox->insertItem( tr("VT 102"), 1 );  // /* , id_term_vt102 */);
    m_terminalBox->insertItem( tr("Linux Console"), 2 ); //, id_term_linux );
    m_terminalBox->insertItem( tr("X-Terminal"), 3 ); //, id_term_xterm );
    //m_terminalBox->insertItem( tr("ANSI"), id_term_ansi );

    m_colorCmb->insertItem( tr("black on white"), id_term_black );
    m_colorCmb->insertItem( tr("white on black"), id_term_white );
    m_colorCmb->insertItem( tr("green on black"), id_term_green );
    m_colorCmb->insertItem( tr("orange on black"), id_term_orange );

    // signals + slots
    /*
    connect(m_terminalBox, SIGNAL(activated(int) ),
            this, SLOT(slotTermTerm(int) ) );
    connect(m_colorBox, SIGNAL(activated(int) ),
            tis, SLOT(slotTermColor(int) ) );
    connect(m_groupSize, SIGNAL(activated(int) ),
            this, SLOT(slotTermFont(int) ) );

    connect(m_optionEcho, SIGNAL(toggled(bool) ),
            this, SLOT(slotTermEcho(bool) ) );
    connect(m_optionWrap, SIGNAL(toggled(bool) ),
            this, SLOT(slotTermWrap(bool) ) );
    connect(m_convInbound, SIGNAL(toggled(bool) ),
            this, SLOT(slotTermInbound(bool) ) );
    connect(m_convOutbound, SIGNAL(toggled(bool) ),
            this, SLOT(slotTermOutbound(bool) ) );
*/
}
TerminalWidget::~TerminalWidget() {
}
void TerminalWidget::load( const Profile& prof ) {
    int term = prof.readNumEntry("Terminal");
    int color = prof.readNumEntry("Color");
    int fontsize = prof.readNumEntry("Font");
    int opt_echo = prof.readNumEntry("Echo");
    int opt_wrap = prof.readNumEntry("Wrap");
    int opt_inbound = prof.readNumEntry("Inbound");
    int opt_outbound = prof.readNumEntry("Outbound");

    switch( term ) {
    case Profile::VT100:
        m_terminalBox->setCurrentItem(id_term_vt100 );
        break;
    case Profile::VT102:
        m_terminalBox->setCurrentItem(id_term_vt102 );
        break;
    case Profile::Linux:
        m_terminalBox->setCurrentItem(id_term_linux );
        break;
    case Profile::XTerm:
        m_terminalBox->setCurrentItem(id_term_xterm );
        break;
    default:
        m_terminalBox->setCurrentItem(id_term_vt102 );
        break;
    };

    switch( color ) {
    case Profile::Black:
        m_colorCmb->setCurrentItem(id_term_black );
        break;
    case Profile::White:
        m_colorCmb->setCurrentItem(id_term_white );
        break;
    case Profile::Green:
        m_colorCmb->setCurrentItem(id_term_green );
        break;
    case Profile::Orange:
        m_colorCmb->setCurrentItem(id_term_orange );
        break;
    default:
        break;
    };

    switch( fontsize ) {
    case Profile::Micro:
        m_sizeSmall->setChecked(true );
        break;
    case Profile::Small:
        m_sizeMedium->setChecked(true );
        break;
    case Profile::Medium:
        m_sizeLarge->setChecked( true );
        break;
    default:
        m_sizeMedium->setChecked(true );
        break;
    };

    if (opt_echo) m_optionEcho->setChecked( true );
    if (opt_wrap) m_optionWrap->setChecked( true );
    if (opt_inbound) m_convInbound->setChecked( true );
    if (opt_outbound) m_convOutbound->setChecked( true );

}
void TerminalWidget::save( Profile& profile ) {
    switch(m_terminalBox->currentItem() ) {
    case id_term_vt100:
        profile.writeEntry("Terminal", Profile::VT100 );
        break;
    case id_term_vt102:
        profile.writeEntry("Terminal", Profile::VT102 );
        break;
    case id_term_linux:
        profile.writeEntry("Terminal", Profile::Linux );
        break;
    case id_term_xterm:
        profile.writeEntry("Terminal", Profile::XTerm );
        break;
    //case id_term_ansi:
    //    profile.writeEntry("Terminal", Profile::VT102 );
    //    break;
    default:
        break;
    };

    // color
    switch(m_colorCmb->currentItem() ) {
    case id_term_black:
        profile.writeEntry("Color", Profile::Black );
        break;
    case id_term_white:
        profile.writeEntry("Color", Profile::White );
        break;
    case id_term_green:
        profile.writeEntry("Color", Profile::Green );
        break;
    case id_term_orange:
        profile.writeEntry("Color", Profile::Orange );
        break;
    default:
        break;
    };

    if (m_sizeSmall->isChecked() ) {
        profile.writeEntry("Font", Profile::Micro );
    }else if (m_sizeMedium->isChecked() ) {
        profile.writeEntry("Font", Profile::Small );
    }else {
        profile.writeEntry("Font", Profile::Medium );
    }

    profile.writeEntry("Echo", m_optionEcho->isChecked() );
    profile.writeEntry("Wrap", m_optionWrap->isChecked() );
    profile.writeEntry("Inbound", m_convInbound->isChecked() );
    profile.writeEntry("Outbound",m_convOutbound->isChecked() );
}
