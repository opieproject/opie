#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qscrollview.h>

#include "metafactory.h"
#include "profileeditordialog.h"

namespace {
    void setCurrent( const QString& str, QComboBox* bo ) {
        for (int i = 0; i < bo->count(); i++ ) {
            if ( bo->text(i) == str ) {
                bo->setCurrentItem( i );
            }
        }
    };
}

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact,
                                          const Profile& prof )
    : QDialog(0, 0, TRUE), m_fact( fact ), m_prof( prof )
{
    initUI();

	// Apply current profile
	// plugin_plugin->load(profile);
	// ... (reset profile name line edit etc.)
}

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact )
    : QDialog(0, 0, TRUE), m_fact( fact )
{
	// Default profile
	m_prof = Profile("New Profile", "serial", "default", Profile::Black, Profile::White, Profile::VT102);

	initUI();

	// Apply current profile
	// plugin_plugin->load(profile);
}

Profile ProfileEditorDialog::profile() const
{
  	return m_prof;
}

void ProfileEditorDialog::initUI()
{
    m_con = m_term = m_key = 0l;


    QVBoxLayout *mainLayout = new QVBoxLayout( this );
    tabWidget = new OTabWidget( this );
    tabWidget->setTabStyle(OTabWidget::TextTab);
    mainLayout->add(tabWidget);

    /* base tabs */
    tabprof   = new QWidget(this);
    m_tabTerm = new QWidget(this);
    m_tabCon  = new QWidget(this);
    m_tabKey  = new QWidget(this);

    m_svCon = new QScrollView( m_tabCon );
    m_svCon->setResizePolicy( QScrollView::AutoOneFit );
    //m_svCon->setHScrollBarMode( QScrollView::AlwaysOff );
    m_svCon->setFrameShape( QFrame::NoFrame );
    m_svTerm = new QScrollView( m_tabTerm );
    m_svTerm->setResizePolicy( QScrollView::AutoOneFit );
    //m_svTerm->setHScrollBarMode( QScrollView::AlwaysOff );
    m_svTerm->setFrameShape( QFrame::NoFrame );

    /* base layout for tabs */
    m_layCon  = new QHBoxLayout( m_tabCon , 2 );
    m_layTerm = new QHBoxLayout( m_tabTerm, 2 );
    m_layKey = new QHBoxLayout( m_tabKey, 2 );

    m_layCon->addWidget( m_svCon );
    m_layTerm->addWidget( m_svTerm );

    // profile tab

    QLabel *name = new QLabel(QObject::tr("Profile name"), tabprof);
    m_name = new QLineEdit(tabprof);
    QLabel *con = new QLabel(tr("Connection"), tabprof );
    QLabel *term = new QLabel(tr("Terminal"), tabprof );
    m_conCmb = new QComboBox( tabprof );
    m_termCmb = new QComboBox( tabprof );
    m_autoConnect = new QCheckBox(tr("Auto connect after load"), tabprof);

    // layouting
    QVBoxLayout *vbox3 = new QVBoxLayout(tabprof, 2);
    vbox3->add(name);
    vbox3->add(m_name);
    vbox3->add(con );
    vbox3->add(m_conCmb );
    vbox3->add(term );
    vbox3->add(m_termCmb );
    vbox3->add(m_autoConnect);
    vbox3->addStretch(1);

    m_showconntab = 0;
    tabWidget->addTab(tabprof, "", QObject::tr("Profile"));
    tabWidget->addTab(m_tabCon, "", QObject::tr("Connection"));
    tabWidget->addTab(m_tabTerm, "", QObject::tr("Terminal"));
    tabWidget->addTab(m_tabKey, "", QObject::tr("Special Keys"));
    tabWidget->setCurrentTab( tabprof );


    // fill the comboboxes
    QStringList list = m_fact->connectionWidgets();
    QStringList::Iterator it;
    for (it =list.begin(); it != list.end(); ++it ) {
        m_conCmb->insertItem( (*it) );
    }
    list = m_fact->terminalWidgets();
    for (it =list.begin(); it != list.end(); ++it ) {
        m_termCmb->insertItem( (*it) );
    }

    // load profile values
    m_name->setText(m_prof.name());
    slotConActivated(  m_fact->external(m_prof.ioLayerName()  ) );
    slotTermActivated( m_fact->external(m_prof.terminalName() ) );
    slotKeyActivated( "Default Keyboard" );
    setCurrent( m_fact->external(m_prof.ioLayerName() ), m_conCmb );
    setCurrent( m_fact->external(m_prof.terminalName() ), m_termCmb );
    m_autoConnect->setChecked(m_prof.autoConnect());


    // signal and slots
    connect(m_conCmb, SIGNAL(activated(const QString& ) ),
            this, SLOT(slotConActivated(const QString&) ) );
    connect(m_termCmb, SIGNAL(activated(const QString& ) ),
            this, SLOT(slotTermActivated(const QString& ) ) );

}

ProfileEditorDialog::~ProfileEditorDialog() {

}
void ProfileEditorDialog::accept()
{
	if(profName().isEmpty())
	{
		QMessageBox::information(this,
			QObject::tr("Invalid profile"),
			QObject::tr("Please enter a profile name."));
		return;
	}
	// Save profile and plugin profile
	//if(plugin_plugin) plugin_plugin->save();

	// Save general values
	m_prof.setName( profName() );
	m_prof.setIOLayer( m_fact->internal(m_conCmb ->currentText()  ) );
        m_prof.setTerminalName( m_fact->internal(m_termCmb->currentText()  ) );
        m_prof.setAutoConnect( m_autoConnect->isChecked() );

        if (m_con )
            m_con->save( m_prof );
        if (m_term )
            m_term->save( m_prof );
        if (m_key)
            m_key->save( m_prof );

	QDialog::accept();
}


QString ProfileEditorDialog::profName()const
{
	return m_name->text();
}

QCString ProfileEditorDialog::profType()const
{
    /*QStringList w = m_fact->configWidgets();
	for(QStringList::Iterator it = w.begin(); it != w.end(); it++)
		if(device_box->currentText() == m_fact->name((*it))) return (*it);
    */
    return QCString();
}
/*
 * we need to switch the widget
 */
void ProfileEditorDialog::slotConActivated( const QString& str ) {

    delete m_con;

    m_con = m_fact->newConnectionPlugin( str, m_svCon->viewport() );

    if ( !m_con ) {
        m_con = new NoOptions( str, m_svCon->viewport(), "name");
    }

    // FIXME ugly hack right. Right solution would be to look into the layer and see if it
    // supports auto connect and then set it as prefered
    //if (  (  )->layer()->supports()[0] == 1 ) {
    if ( m_conCmb ->currentText() == tr("Local Console") ) {
        m_autoConnect->setChecked( true );
    } else {
        m_autoConnect->setChecked( false );
    }

    m_con->load( m_prof );
    m_svCon->addChild( m_con );
}


/*
 * we need to switch the widget
 */
void ProfileEditorDialog::slotTermActivated( const QString& str ) {

    delete m_term;

    m_term = m_fact->newTerminalPlugin( str, m_svTerm->viewport() );

    if (m_term) {
        m_term->load( m_prof );
        m_svTerm->addChild( m_term );
    }
}

void ProfileEditorDialog::slotKeyActivated(const QString &str) {
    delete m_key;
    m_key = m_fact->newKeyboardPlugin( str, m_tabKey );

    if (m_key) {

        m_key->load(m_prof);
        m_layKey->addWidget(m_key);
    }

}
