
#include "profileeditorplugins.h"
#include "profile.h"

#include "qframe.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qlayout.h"

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

