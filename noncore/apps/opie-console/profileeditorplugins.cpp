
#include "profileeditorplugins.h"
#include "profile.h"

#include "qframe.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qlayout.h"
#include "qcombobox.h"
#include "qradiobutton.h"
#include "qcheckbox.h"
#include "qbuttongroup.h"

#include "io_serial.h"

// Base class

ProfileEditorPlugin::ProfileEditorPlugin(QWidget *parent, Profile p)
{
	m_parent = parent;
	m_profile = p;
	m_widget = NULL;
}

ProfileEditorPlugin::~ProfileEditorPlugin()
{
	if(m_widget) delete m_widget;
}

QWidget *ProfileEditorPlugin::connection_widget()
{
	QWidget *root;
	QVBoxLayout *lroot;

	root = new QWidget();

	// Build GUI

	QComboBox *speed_box = new QComboBox(root);
	speed_box->insertItem("115200 baud", id_baud_115200);
	speed_box->insertItem("57600 baud", id_baud_57600);
	speed_box->insertItem("38400 baud", id_baud_38400);
	speed_box->insertItem("19200 baud", id_baud_19200);
	speed_box->insertItem("9600 baud", id_baud_9600);

	QLabel *speedlabel = new QLabel(QObject::tr("Speed"), root);
	QLabel *flow = new QLabel(QObject::tr("Flow control"), root);
	QLabel *parity = new QLabel(QObject::tr("Parity"), root);

	QButtonGroup *group_flow = new QButtonGroup(root);
	group_flow->hide();
	QRadioButton *flow_hw = new QRadioButton(QObject::tr("Hardware"), root);
	QRadioButton *flow_sw = new QRadioButton(QObject::tr("Software"), root);
	group_flow->insert(flow_hw, id_flow_hw);
	group_flow->insert(flow_sw, id_flow_sw);

	QButtonGroup *group_parity = new QButtonGroup(root);
	group_parity->hide();
	QRadioButton *parity_odd = new QRadioButton(QObject::tr("Odd"), root);
	QRadioButton *parity_even = new QRadioButton(QObject::tr("Even"), root);
	group_parity->insert(parity_odd, id_parity_odd);
	group_parity->insert(parity_even, id_parity_even);

	// Build Layout

	lroot = new QVBoxLayout(root);
	lroot->add(speedlabel);
	lroot->add(speed_box);
	lroot->add(flow);
	QHBoxLayout *hbox = new QHBoxLayout(lroot, 2);
	hbox->add(flow_hw);
	hbox->add(flow_sw);
	lroot->add(parity);
	QHBoxLayout *hbox2 = new QHBoxLayout(lroot, 2);
	hbox2->add(parity_odd);
	hbox2->add(parity_even);

	// Apply profile settings
	int rad_flow = m_profile.readNumEntry("Flow");
	int rad_parity = m_profile.readNumEntry("Parity");
	int speed = m_profile.readNumEntry("Speed");

	if(rad_flow == IOSerial::FlowHW) flow_hw->setChecked(true);
	else flow_sw->setChecked(true);
	if(rad_parity == IOSerial::ParityEven) parity_even->setChecked(true);
	else parity_odd->setChecked(true);
	if(speed == 115200) speed_box->setCurrentItem(id_baud_115200);
	if(speed == 57600) speed_box->setCurrentItem(id_baud_57600);
	if(speed == 38400) speed_box->setCurrentItem(id_baud_38400);
	if(speed == 19200) speed_box->setCurrentItem(id_baud_19200);
	if(speed == 9600) speed_box->setCurrentItem(id_baud_9600);

	// Signals

	connect(group_flow, SIGNAL(clicked(int)), SLOT(slotConnFlow(int)));
	connect(group_parity, SIGNAL(clicked(int)), SLOT(slotConnParity(int)));
	connect(speed_box, SIGNAL(activated(int)), SLOT(slotConnSpeed(int)));

	return root;
}

QWidget *ProfileEditorPlugin::terminal_widget()
{
	QWidget *root;
	QVBoxLayout *lroot;

	root = new QWidget();

	// Build GUI

	QComboBox *terminal_box = new QComboBox(root);
	terminal_box->insertItem("VT 100", id_term_vt100);
	terminal_box->insertItem("VT 220", id_term_vt220);
	terminal_box->insertItem("ANSI", id_term_ansi);

	QLabel *terminal = new QLabel(QObject::tr("Terminal type"), root);
	QLabel *colourlabel = new QLabel(QObject::tr("Colour scheme"), root);
	QLabel *sizelabel = new QLabel(QObject::tr("Font size"), root);
	QLabel *options = new QLabel(QObject::tr("Options"), root);
	QLabel *conversions = new QLabel(QObject::tr("Line-break conversions"), root);

	QComboBox *colour_box = new QComboBox(root);
	colour_box->insertItem(QObject::tr("black on white"), id_term_black);
	colour_box->insertItem(QObject::tr("white on black"), id_term_white);

	QButtonGroup *group_size = new QButtonGroup(root);
	group_size->hide();
	QRadioButton *size_small = new QRadioButton(QObject::tr("small"), root);
	QRadioButton *size_medium = new QRadioButton(QObject::tr("medium"), root);
	QRadioButton *size_large = new QRadioButton(QObject::tr("large"), root);
	group_size->insert(size_small);
	group_size->insert(size_medium);
	group_size->insert(size_large);

	QCheckBox *option_echo = new QCheckBox(QObject::tr("Local echo"), root);
	QCheckBox *option_wrap = new QCheckBox(QObject::tr("Line wrap"), root);

	QCheckBox *conv_inbound = new QCheckBox(QObject::tr("Inbound"), root);
	QCheckBox *conv_outbound = new QCheckBox(QObject::tr("Outbound"), root);

	// Build Layout

	lroot = new QVBoxLayout(root, 2);
	lroot->add(terminal);
	lroot->add(terminal_box);
	lroot->add(sizelabel);
	QHBoxLayout *hbox = new QHBoxLayout(lroot, 2);
	hbox->add(size_small);
	hbox->add(size_medium);
	hbox->add(size_large);
	lroot->add(colourlabel);
	lroot->add(colour_box);
	lroot->add(conversions);
	QHBoxLayout *hbox2 = new QHBoxLayout(lroot, 2);
	hbox2->add(conv_inbound);
	hbox2->add(conv_outbound);
	lroot->add(options);
	QHBoxLayout *hbox3 = new QHBoxLayout(lroot, 2);
	hbox3->add(option_wrap);
	hbox3->add(option_echo);

	// Apply profile settings
	int term = m_profile.readNumEntry("Terminal");
	int colour = m_profile.readNumEntry("Colour");
	int fontsize = m_profile.readNumEntry("Font");
	int opt_echo = m_profile.readNumEntry("Echo");
	int opt_wrap = m_profile.readNumEntry("Wrap");
	int opt_inbound = m_profile.readNumEntry("Inbound");
	int opt_outbound = m_profile.readNumEntry("Outbound");

	if(term == Profile::VT102) terminal_box->setCurrentItem(id_term_vt100);

	if(colour == Profile::Black) colour_box->setCurrentItem(id_term_black);
	if(colour == Profile::White) colour_box->setCurrentItem(id_term_white);

	if(fontsize == Profile::Micro) size_small->setChecked(true);
	if(fontsize == Profile::Small) size_medium->setChecked(true);
	if(fontsize == Profile::Medium) size_large->setChecked(true);

	if(opt_echo) option_echo->setChecked(true);
	if(opt_wrap) option_wrap->setChecked(true);
	if(opt_inbound) conv_inbound->setChecked(true);
	if(opt_outbound) conv_outbound->setChecked(true);

	// Signals

	connect(terminal_box, SIGNAL(activated(int)), SLOT(slotTermTerm(int)));
	connect(colour_box, SIGNAL(activated(int)), SLOT(slotTermColour(int)));
	connect(group_size, SIGNAL(clicked(int)), SLOT(slotTermFont(int)));

	connect(option_echo, SIGNAL(toggled(bool)), SLOT(slotTermEcho(bool)));
	connect(option_wrap, SIGNAL(toggled(bool)), SLOT(slotTermWrap(bool)));
	connect(conv_inbound, SIGNAL(toggled(bool)), SLOT(slotTermInbound(bool)));
	connect(conv_outbound, SIGNAL(toggled(bool)), SLOT(slotTermOutbound(bool)));

	return root;
}

void ProfileEditorPlugin::slotConnFlow(int id)
{
	switch(id)
	{
		case id_flow_hw:
			m_profile.writeEntry("Flow", IOSerial::FlowHW);
			break;
		case id_flow_sw:
			m_profile.writeEntry("Flow", IOSerial::FlowSW);
			break;
	}
}

void ProfileEditorPlugin::slotConnParity(int id)
{
	switch(id)
	{
		case id_parity_odd:
			m_profile.writeEntry("Parity", IOSerial::ParityEven);
			break;
		case id_parity_even:
			m_profile.writeEntry("Parity", IOSerial::ParityOdd);
			break;
	}
}

void ProfileEditorPlugin::slotConnSpeed(int id)
{
	switch(id)
	{

		case id_baud_115200:
			m_profile.writeEntry("Speed", 115200);
			break;
		case id_baud_57600:
			m_profile.writeEntry("Speed", 57600);
			break;
		case id_baud_38400:
			m_profile.writeEntry("Speed", 38400);
			break;
		case id_baud_19200:
			m_profile.writeEntry("Speed", 19200);
			break;
		case id_baud_9600:
			m_profile.writeEntry("Speed", 9600);
			break;
	}
}

void ProfileEditorPlugin::slotTermTerm(int id)
{
	switch(id)
	{
		case id_term_vt100:
			m_profile.writeEntry("Terminal", Profile::VT102);
			break;
		case id_term_vt220:
			m_profile.writeEntry("Terminal", Profile::VT102);
			break;
		case id_term_ansi:
			m_profile.writeEntry("Terminal", Profile::VT102);
			break;
	}
}

void ProfileEditorPlugin::slotTermColour(int id)
{
	switch(id)
	{
		case id_term_black:
			m_profile.writeEntry("Colour", Profile::Black);
			break;
		case id_term_white:
			m_profile.writeEntry("Colour", Profile::White);
			break;
	}
}

void ProfileEditorPlugin::slotTermFont(int id)
{
	switch(id)
	{
		case id_size_small:
			m_profile.writeEntry("Font", Profile::Micro);
			break;
		case id_size_medium:
			m_profile.writeEntry("Font", Profile::Small);
			break;
		case id_size_large:
			m_profile.writeEntry("Font", Profile::Medium);
			break;
	}
}

void ProfileEditorPlugin::slotTermEcho(bool on)
{
	m_profile.writeEntry("Echo", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermWrap(bool on)
{
	m_profile.writeEntry("Wrap", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermInbound(bool on)
{
	m_profile.writeEntry("Inbound", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermOutbound(bool on)
{
	m_profile.writeEntry("Outbound", on ? 1 : 0);
}

// Inherited classes

class ProfileEditorPluginSerial : public ProfileEditorPlugin
{
	public:
	
	ProfileEditorPluginSerial(QWidget *parent, Profile p)
	: ProfileEditorPlugin(parent, p)
	{
	}

	~ProfileEditorPluginSerial()
	{
	}

	QWidget *widget()
	{
		if(!m_widget)
		{
			QFrame *device_frame = new QFrame(m_parent);
			device_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);

			QLabel *frame_device = new QLabel(QObject::tr("Device"), device_frame);

			device_line = new QLineEdit("/dev/ttyS0", device_frame);

			QVBoxLayout *vbox_frame = new QVBoxLayout(device_frame, 2);
			vbox_frame->add(frame_device);
			vbox_frame->add(device_line);

			m_widget = device_frame;

			// Load special settings
			device_line->setText(m_profile.readEntry("Device"));
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		Profile p = m_profile;
		p.writeEntry("Device", device_line->text());
	}

	private:
		QLineEdit *device_line;
};

class ProfileEditorPluginIrda : public ProfileEditorPlugin
{
	public:

	ProfileEditorPluginIrda(QWidget *parent, Profile p)
	: ProfileEditorPlugin(parent, p)
	{
	}

	~ProfileEditorPluginIrda()
	{
	}

	QWidget *widget()
	{
		if(!m_widget)
		{
			QFrame *device_frame = new QFrame(m_parent);
			device_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);

			QLabel *frame_device = new QLabel(QObject::tr("Device"), device_frame);

			device_line = new QLineEdit("/dev/ircomm0", device_frame);

			QVBoxLayout *vbox_frame = new QVBoxLayout(device_frame, 2);
			vbox_frame->add(frame_device);
			vbox_frame->add(device_line);

			m_widget = device_frame;

			// Load special settings
			device_line->setText(m_profile.readEntry("Device"));
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		Profile p = m_profile;
		p.writeEntry("Device", device_line->text());
	}

	private:
		QLineEdit *device_line;
};

class ProfileEditorPluginModem : public ProfileEditorPlugin
{
	public:

	ProfileEditorPluginModem(QWidget *parent, Profile p)
	: ProfileEditorPlugin(parent, p)
	{
	}

	~ProfileEditorPluginModem()
	{
	}

	QWidget *widget()
	{
		if(!m_widget)
		{
			QFrame *device_frame = new QFrame(m_parent);
			device_frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);

			QLabel *frame_device = new QLabel(QObject::tr("Device"), device_frame);
			QLabel *frame_number = new QLabel(QObject::tr("Phone number"), device_frame);

			device_line = new QLineEdit("/dev/ttyS0", device_frame);
			number_line = new QLineEdit(device_frame);

			QVBoxLayout *vbox_frame = new QVBoxLayout(device_frame, 2);
			vbox_frame->add(frame_device);
			vbox_frame->add(device_line);
			vbox_frame->add(frame_number);
			vbox_frame->add(number_line);

			m_widget = device_frame;

			// Load special settings
			device_line->setText(m_profile.readEntry("Device"));
			number_line->setText(m_profile.readEntry("Number"));
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		Profile p = m_profile;
		p.writeEntry("Device", device_line->text());
		p.writeEntry("Number", number_line->text());
	}

	private:
		QLineEdit *device_line, *number_line;
};

ProfileEditorPlugin *factory_serial(QWidget *parent, const Profile& p)
{
	return new ProfileEditorPluginSerial(parent, p);
}

ProfileEditorPlugin *factory_irda(QWidget *parent, const Profile& p)
{
	return new ProfileEditorPluginIrda(parent, p);
}

ProfileEditorPlugin *factory_modem(QWidget *parent, const Profile& p)
{
	return new ProfileEditorPluginModem(parent, p);
}

