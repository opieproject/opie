
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qhgroupbox.h>
#include <qvbox.h>

#include "io_serial.h"
#include "profile.h"
#include "profileeditorplugins.h"

// Base class

ProfileEditorPlugin::ProfileEditorPlugin(QWidget *parent, Profile *p)
: QObject()
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
	QLabel *speedlabel = new QLabel(QObject::tr("Speed"), root);

	QComboBox *speed_box = new QComboBox(root);
	speed_box->insertItem("115200 baud", id_baud_115200);
	speed_box->insertItem("57600 baud", id_baud_57600);
	speed_box->insertItem("38400 baud", id_baud_38400);
	speed_box->insertItem("19200 baud", id_baud_19200);
	speed_box->insertItem("9600 baud", id_baud_9600);

	QButtonGroup *group_flow = new QButtonGroup(QObject::tr("Flow control"), root);

	QRadioButton *flow_hw = new QRadioButton(QObject::tr("Hardware"), group_flow);
	QRadioButton *flow_sw = new QRadioButton(QObject::tr("Software"), group_flow);

	QButtonGroup *group_parity = new QButtonGroup(QObject::tr("Parity"), root);
	QRadioButton *parity_odd = new QRadioButton(QObject::tr("Odd"), group_parity);
	QRadioButton *parity_even = new QRadioButton(QObject::tr("Even"), group_parity);

	// Build Layout
	lroot = new QVBoxLayout(root);
	lroot->add(speedlabel);
	lroot->add(speed_box);
        lroot->setStretchFactor(speedlabel, 1);
        lroot->setStretchFactor(speed_box, 1);

	QHBoxLayout *hbox = new QHBoxLayout(group_flow, 2);
	hbox->add(flow_hw);
	hbox->add(flow_sw);
        lroot->add(group_flow);
        lroot->setStretchFactor(group_flow, 2);

        QHBoxLayout *hboxPar = new QHBoxLayout( group_parity, 2);
	hboxPar->add(parity_odd);
        hboxPar->add(parity_even);
        lroot->add(group_parity);
        lroot->setStretchFactor(group_parity, 2);
	// Apply profile settings

	int rad_flow = m_profile->readNumEntry("Flow");
	int rad_parity = m_profile->readNumEntry("Parity");
	int speed = m_profile->readNumEntry("Speed");

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


	QLabel *terminal = new QLabel(QObject::tr("Terminal type"), root);

	QComboBox *terminal_box = new QComboBox(root);
	terminal_box->insertItem("VT 100", id_term_vt100);
	terminal_box->insertItem("VT 220", id_term_vt220);
	terminal_box->insertItem("ANSI", id_term_ansi);

        QLabel *colourlabel = new QLabel(QObject::tr("Colour scheme"), root);
	QComboBox *colour_box = new QComboBox(root);
	colour_box->insertItem(QObject::tr("black on white"), id_term_black);
	colour_box->insertItem(QObject::tr("white on black"), id_term_white);

	QButtonGroup *group_size = new QButtonGroup( QObject::tr("Font size"), root );
	QRadioButton *size_small = new QRadioButton(QObject::tr("small"), group_size );
	QRadioButton *size_medium = new QRadioButton(QObject::tr("medium"), group_size );
	QRadioButton *size_large = new QRadioButton(QObject::tr("large"), group_size );

        QHGroupBox *group_conv = new QHGroupBox( QObject::tr("Line-break conversions"), root );
	QCheckBox *conv_inbound = new QCheckBox(QObject::tr("Inbound"), group_conv);
	QCheckBox *conv_outbound = new QCheckBox(QObject::tr("Outbound"), group_conv);

        QHGroupBox *group_options = new QHGroupBox( QObject::tr("Options"), root );
 	QCheckBox *option_echo = new QCheckBox(QObject::tr("Local echo"), group_options);
	QCheckBox *option_wrap = new QCheckBox(QObject::tr("Line wrap"), group_options);

	// Build Layout
	lroot = new QVBoxLayout(root, 2);

        QVBoxLayout *typeBox = new QVBoxLayout( lroot );
        typeBox->add(terminal);
        typeBox->add(terminal_box);

	QHBoxLayout *hbox = new QHBoxLayout( group_size, 2);
	hbox->add(size_small);
	hbox->add(size_medium);
	hbox->add(size_large);
        lroot->add( group_size );

        QVBoxLayout *colourBox = new QVBoxLayout( lroot );
        colourBox->add(colourlabel);
        colourBox->add(colour_box);

        lroot->add(group_conv);
        lroot->add(group_options);

	// Apply profile settings

	int term = m_profile->readNumEntry("Terminal");
	int colour = m_profile->readNumEntry("Colour");
	int fontsize = m_profile->readNumEntry("Font");
	int opt_echo = m_profile->readNumEntry("Echo");
	int opt_wrap = m_profile->readNumEntry("Wrap");
	int opt_inbound = m_profile->readNumEntry("Inbound");
	int opt_outbound = m_profile->readNumEntry("Outbound");

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
        m_profile->writeEntry("Flow", IOSerial::FlowHW);
        break;
    case id_flow_sw:
        m_profile->writeEntry("Flow", IOSerial::FlowSW);
        break;
    case id_flow_sw:
        m_profile->writeEntry("None", IOSerial::None);
        break;
    }
}

void ProfileEditorPlugin::slotConnParity(int id)
{
	switch(id)
	{
		case id_parity_odd:
			m_profile->writeEntry("Parity", IOSerial::ParityEven);
			break;
		case id_parity_even:
			m_profile->writeEntry("Parity", IOSerial::ParityOdd);
			break;
	}
}

void ProfileEditorPlugin::slotConnSpeed(int id)
{
	switch(id)
	{

		case id_baud_115200:
			m_profile->writeEntry("Speed", 115200);
			break;
		case id_baud_57600:
			m_profile->writeEntry("Speed", 57600);
			break;
		case id_baud_38400:
			m_profile->writeEntry("Speed", 38400);
			break;
		case id_baud_19200:
			m_profile->writeEntry("Speed", 19200);
			break;
		case id_baud_9600:
			m_profile->writeEntry("Speed", 9600);
			break;
	}
}

void ProfileEditorPlugin::slotTermTerm(int id)
{
	switch(id)
	{
		case id_term_vt100:
			m_profile->writeEntry("Terminal", Profile::VT102);
			break;
		case id_term_vt220:
			m_profile->writeEntry("Terminal", Profile::VT102);
			break;
		case id_term_ansi:
			m_profile->writeEntry("Terminal", Profile::VT102);
			break;
	}
}

void ProfileEditorPlugin::slotTermColour(int id)
{
	switch(id)
	{
		case id_term_black:
			m_profile->writeEntry("Colour", Profile::Black);
			break;
		case id_term_white:
			m_profile->writeEntry("Colour", Profile::White);
			break;
	}
}

void ProfileEditorPlugin::slotTermFont(int id)
{
	switch(id)
	{
		case id_size_small:
			m_profile->writeEntry("Font", Profile::Micro);
			break;
		case id_size_medium:
			m_profile->writeEntry("Font", Profile::Small);
			break;
		case id_size_large:
			m_profile->writeEntry("Font", Profile::Medium);
			break;
	}
}

void ProfileEditorPlugin::slotTermEcho(bool on)
{
	m_profile->writeEntry("Echo", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermWrap(bool on)
{
	m_profile->writeEntry("Wrap", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermInbound(bool on)
{
	m_profile->writeEntry("Inbound", on ? 1 : 0);
}

void ProfileEditorPlugin::slotTermOutbound(bool on)
{
	m_profile->writeEntry("Outbound", on ? 1 : 0);
}

// Inherited classes

class ProfileEditorPluginSerial : public ProfileEditorPlugin
{
	public:

	ProfileEditorPluginSerial(QWidget *parent, Profile *p)
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

                    QWidget *device_frame = new QWidget( m_parent );
                    QLabel *frame_device = new QLabel(QObject::tr("Device"), device_frame);

                    device_line = new QLineEdit("/dev/ttyS0", device_frame);

                    QVBoxLayout *vbox_frame = new QVBoxLayout(device_frame, 2);
                    vbox_frame->add(frame_device);
                    vbox_frame->add(device_line);

                    m_widget = device_frame;

                    // Load special settings

                    QString dev = m_profile->readEntry("Device");
                    if(!dev.isNull()) device_line->setText(dev);
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		m_profile->writeEntry("Device", device_line->text());
	}

	private:
		QLineEdit *device_line;
};

class ProfileEditorPluginIrda : public ProfileEditorPlugin
{
	public:

	ProfileEditorPluginIrda(QWidget *parent, Profile *p)
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
			QWidget *device_frame = new QWidget(m_parent);


			QLabel *frame_device = new QLabel(QObject::tr("Device"), device_frame);

			device_line = new QLineEdit("/dev/ircomm0", device_frame);

			QVBoxLayout *vbox_frame = new QVBoxLayout(device_frame, 2);
			vbox_frame->add(frame_device);
			vbox_frame->add(device_line);

			m_widget = device_frame;

			// Load special settings
			QString dev = m_profile->readEntry("Device");
			if(!dev.isNull()) device_line->setText(dev);
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		m_profile->writeEntry("Device", device_line->text());
	}

	private:
		QLineEdit *device_line;
};

class ProfileEditorPluginModem : public ProfileEditorPlugin
{
	public:

	ProfileEditorPluginModem(QWidget *parent, Profile *p)
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
			QWidget *device_frame = new QWidget(m_parent);

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
			QString dev = m_profile->readEntry("Device");
			QString num = m_profile->readEntry("Number");
			if(!dev.isNull()) device_line->setText(dev);
			number_line->setText(num);
		}

		return m_widget;
	}

	void save()
	{
		// special settings
		m_profile->writeEntry("Device", device_line->text());
		m_profile->writeEntry("Number", number_line->text());
	}

	private:
		QLineEdit *device_line, *number_line;
};

ProfileEditorPlugin *factory_serial(QWidget *parent, Profile *p)
{
	return new ProfileEditorPluginSerial(parent, p);
}

ProfileEditorPlugin *factory_irda(QWidget *parent, Profile *p)
{
	return new ProfileEditorPluginIrda(parent, p);
}

ProfileEditorPlugin *factory_modem(QWidget *parent, Profile *p)
{
	return new ProfileEditorPluginModem(parent, p);
}

