
#include "profileeditordialog.h"

#include "qlayout.h"
#include "qlineedit.h"
#include "qlabel.h"
#include "qmessagebox.h"
#include "qstringlist.h"
#include "qcombobox.h"

#include "profileeditorplugins.h"
#include "metafactory.h"

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact,
                                          const Profile& prof )
    : QTabDialog(0, 0, TRUE), m_fact( fact ), m_prof( prof )
{
    initUI();

	// Apply current profile
	// plugin_plugin->load(profile);
	// ... (reset profile name line edit etc.)
}

ProfileEditorDialog::ProfileEditorDialog( MetaFactory* fact )
    : QTabDialog(0, 0, TRUE), m_fact( fact )
{
	// Default profile
	m_prof = Profile(QString::null, "serial", Profile::Black, Profile::White, Profile::VT102);

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
	QWidget *tabterm, *tabconn, *tabprof;

	tabprof = new QWidget(this);
	tabterm = new QWidget(this);
	tabconn = new QWidget(this);

	// for the time being: fake factory
	
	m_fact->addConfigWidgetFactory("serial", QObject::tr("Serial cable"), factory_serial);
	m_fact->addConfigWidgetFactory("irda", QObject::tr("IrDA port"), factory_irda);
	m_fact->addConfigWidgetFactory("modem", QObject::tr("Serial via modem"), factory_modem);

	// profile tab

	QLabel *name = new QLabel(QObject::tr("Profile name"), tabprof);

	name_line = new QLineEdit(tabprof);

	// connection tab, fixed part

	QLabel *device = new QLabel(QObject::tr("Device"), tabconn);

	device_box = new QComboBox(tabconn);

	QStringList w = m_fact->configWidgets();
	for(QStringList::Iterator it = w.begin(); it != w.end(); it++)
		device_box->insertItem(m_fact->name((*it)));

	// connection tab, factory part
	plugin_base = new QWidget(tabconn);
	plugin_layout = new QHBoxLayout(plugin_base, 0);

	plugin_plugin = m_fact->newConfigPlugin("serial", plugin_base, m_prof);
	plugin_layout->add(plugin_plugin->widget());

	// connection tab, general part

	QWidget *conn_widget = plugin_plugin->connection_widget();
	conn_widget->reparent(tabconn, 0, QPoint(), true);

	// terminal tab

	QWidget *term_widget = plugin_plugin->terminal_widget();
	term_widget->reparent(tabterm, 0, QPoint(), true);

	// layouting

	QVBoxLayout *vbox3 = new QVBoxLayout(tabprof, 2);
	vbox3->add(name);
	vbox3->add(name_line);
	vbox3->addStretch(1);

	QVBoxLayout *vbox = new QVBoxLayout(tabconn, 2);
	vbox->add(device);
	vbox->add(device_box);
	vbox->add(plugin_base);
	vbox->add(conn_widget);

	QVBoxLayout *vbox2 = new QVBoxLayout(tabterm, 2);
	vbox2->add(term_widget);

	addTab(tabprof, QObject::tr("Profile"));
	addTab(tabconn, QObject::tr("Connection"));
	addTab(tabterm, QObject::tr("Terminal"));

	setOkButton(QObject::tr("OK"));
	setCancelButton(QObject::tr("Cancel"));

	// load profile values

	name_line->setText(m_prof.name());

	// signals

	connect(this, SIGNAL(cancelButtonPressed()), SLOT(slotCancel()));
	connect(device_box, SIGNAL(activated(int)), SLOT(slotDevice(int)));
}

ProfileEditorDialog::~ProfileEditorDialog() {

}

void ProfileEditorDialog::slotDevice(int id)
{
	delete plugin_plugin;

	plugin_plugin = m_fact->newConfigPlugin(prof_type(), plugin_base, m_prof);
	plugin_layout->add(plugin_plugin->widget());

	// Reload profile associated to device, including e.g. conn_device()
	// m_prof = plugin_plugin->profile()
	// or, keeping the profile name: m_prof->reload(plugin_plugin->profile())

	//plugin_plugin->show();
	plugin_plugin->widget()->show();
}

void ProfileEditorDialog::accept()
{
	if(prof_name().isEmpty())
	{
		QMessageBox::information(this,
			QObject::tr("Invalid profile"),
			QObject::tr("Please enter a profile name."));
		return;
	}
	// Save profile and plugin profile
	if(plugin_plugin) plugin_plugin->save();

	// Save general values
	m_prof.setName(prof_name());

	QDialog::accept();
}

void ProfileEditorDialog::slotCancel()
{
	reject();
}

QString ProfileEditorDialog::prof_name()
{
	return name_line->text();
}

QString ProfileEditorDialog::prof_type()
{
	QStringList w = m_fact->configWidgets();
	for(QStringList::Iterator it = w.begin(); it != w.end(); it++)
		if(device_box->currentText() == m_fact->name((*it))) return (*it);

	return QString::null;
}


