
#include "profileeditordialog.h"

#include "qlayout.h"
#include "qlineedit.h"
#include "qlabel.h"
#include "qradiobutton.h"
#include "qcombobox.h"
#include "qcheckbox.h"
#include "qmessagebox.h"
#include "qbuttongroup.h"
#include "qstringlist.h"

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

	QComboBox *speed_box = new QComboBox(tabconn);
	speed_box->insertItem("115200 baud", id_baud_115200);
	speed_box->insertItem("57600 baud", id_baud_57600);
	speed_box->insertItem("38400 baud", id_baud_38400);
	speed_box->insertItem("19200 baud", id_baud_19200);
	speed_box->insertItem("9600 baud", id_baud_9600);

	QLabel *speed = new QLabel(QObject::tr("Speed"), tabconn);
	QLabel *flow = new QLabel(QObject::tr("Flow control"), tabconn);
	QLabel *parity = new QLabel(QObject::tr("Parity"), tabconn);

	QButtonGroup *group_flow = new QButtonGroup(tabconn);
	group_flow->hide();
	QRadioButton *flow_hw = new QRadioButton(QObject::tr("Hardware"), tabconn);
	QRadioButton *flow_sw = new QRadioButton(QObject::tr("Software"), tabconn);
	group_flow->insert(flow_hw, id_flow_hw);
	group_flow->insert(flow_sw, id_flow_sw);

	QButtonGroup *group_parity = new QButtonGroup(tabconn);
	group_parity->hide();
	QRadioButton *parity_odd = new QRadioButton(QObject::tr("Odd"), tabconn);
	QRadioButton *parity_even = new QRadioButton(QObject::tr("Even"), tabconn);
	group_parity->insert(parity_odd, id_parity_odd);
	group_parity->insert(parity_even, id_parity_even);

	flow_sw->setChecked(true);
	parity_odd->setChecked(true);

	// terminal tab

	QComboBox *terminal_box = new QComboBox(tabterm);
	terminal_box->insertItem("VT 100", id_term_vt100);
	terminal_box->insertItem("VT 220", id_term_vt220);
	terminal_box->insertItem("ANSI", id_term_ansi);

	QLabel *terminal = new QLabel(QObject::tr("Terminal type"), tabterm);
	QLabel *colour = new QLabel(QObject::tr("Colour scheme"), tabterm);
	QLabel *size = new QLabel(QObject::tr("Font size"), tabterm);
	QLabel *options = new QLabel(QObject::tr("Options"), tabterm);
	QLabel *conversions = new QLabel(QObject::tr("Line-break conversions"), tabterm);

	QComboBox *colour_box = new QComboBox(tabterm);
	colour_box->insertItem(QObject::tr("black on white"));
	colour_box->insertItem(QObject::tr("white on black"));

	QButtonGroup *group_size = new QButtonGroup(tabterm);
	group_size->hide();
	QRadioButton *size_small = new QRadioButton(QObject::tr("small"), tabterm);
	QRadioButton *size_medium = new QRadioButton(QObject::tr("medium"), tabterm);
	QRadioButton *size_large = new QRadioButton(QObject::tr("large"), tabterm);
	group_size->insert(size_small);
	group_size->insert(size_medium);
	group_size->insert(size_large);

	QCheckBox *option_echo = new QCheckBox(QObject::tr("Local echo"), tabterm);
	QCheckBox *option_wrap = new QCheckBox(QObject::tr("Line wrap"), tabterm);

	QCheckBox *conv_inbound = new QCheckBox(QObject::tr("Inbound"), tabterm);
	QCheckBox *conv_outbound = new QCheckBox(QObject::tr("Outbound"), tabterm);

	size_small->setChecked(true);

	// layouting

	QVBoxLayout *vbox3 = new QVBoxLayout(tabprof, 2);
	vbox3->add(name);
	vbox3->add(name_line);
	vbox3->addStretch(1);

	QVBoxLayout *vbox = new QVBoxLayout(tabconn, 2);
	vbox->add(device);
	vbox->add(device_box);
	vbox->add(plugin_base);
	vbox->add(speed);
	vbox->add(speed_box);
	vbox->add(flow);
	QHBoxLayout *hbox = new QHBoxLayout(vbox, 2);
	hbox->add(flow_hw);
	hbox->add(flow_sw);
	//vbox->add(group_flow);
	vbox->add(parity);
	QHBoxLayout *hbox2 = new QHBoxLayout(vbox, 2);
	hbox2->add(parity_odd);
	hbox2->add(parity_even);
	//vbox->add(group_parity);

	QVBoxLayout *vbox2 = new QVBoxLayout(tabterm, 2);
	vbox2->add(terminal);
	vbox2->add(terminal_box);
	vbox2->add(size);
	QHBoxLayout *hbox3 = new QHBoxLayout(vbox2, 2);
	hbox3->add(size_small);
	hbox3->add(size_medium);
	hbox3->add(size_large);
	//vbox2->add(group_size);
	vbox2->add(colour);
	vbox2->add(colour_box);
	vbox2->add(conversions);
	QHBoxLayout *hbox5 = new QHBoxLayout(vbox2, 2);
	hbox5->add(conv_inbound);
	hbox5->add(conv_outbound);
	vbox2->add(options);
	QHBoxLayout *hbox4 = new QHBoxLayout(vbox2, 2);
	hbox4->add(option_wrap);
	hbox4->add(option_echo);

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

	connect(group_flow, SIGNAL(clicked(int)), SLOT(slotConn(int)));
	connect(group_parity, SIGNAL(clicked(int)), SLOT(slotConn(int)));
	connect(speed_box, SIGNAL(clicked(int)), SLOT(slotConn(int)));

	connect(terminal_box, SIGNAL(clicked(int)), SLOT(slotConn(int)));
	connect(group_size, SIGNAL(clicked(int)), SLOT(slotConn(int)));
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

void ProfileEditorDialog::slotConn(int id)
{
	switch(id)
	{
		case id_flow_hw:
			m_prof.writeEntry("Flow", 0x01);
			break;
		case id_flow_sw:
			m_prof.writeEntry("Flow", 0x02);
			break;
		case id_parity_odd:
			m_prof.writeEntry("Parity", 2);
			break;
		case id_parity_even:
			m_prof.writeEntry("Parity", 1);
			break;

		case id_baud_115200:
			m_prof.writeEntry("Speed", 115200);
			break;
		case id_baud_57600:
			m_prof.writeEntry("Speed", 57600);
			break;
		case id_baud_38400:
			m_prof.writeEntry("Speed", 38400);
			break;
		case id_baud_19200:
			m_prof.writeEntry("Speed", 19200);
			break;
		case id_baud_9600:
			m_prof.writeEntry("Speed", 9600);
			break;

		case id_term_vt100:
			m_prof.writeEntry("Terminal", 2);
			break;
		case id_term_vt220:
			m_prof.writeEntry("Terminal", 1);
			break;
		case id_term_ansi:
			m_prof.writeEntry("Terminal", 0);
			break;

		case id_size_small:
			m_prof.writeEntry("Font", 0);
			break;
		case id_size_medium:
			m_prof.writeEntry("Font", 1);
			break;
		case id_size_large:
			m_prof.writeEntry("Font", 2);
			break;
	}
}


