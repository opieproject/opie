#ifndef PROFILE_EDITOR_DIALOG
#define PROFILE_EDITOR_DIALOG

#include <qtabdialog.h>

#include "profile.h"

class MetaFactory;
class EditBase;
class QTabWidget;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QLabel;
class ProfileEditorPlugin;

class ProfileEditorDialog : public QTabDialog {
    Q_OBJECT
public:
    ProfileEditorDialog(MetaFactory* fact,
                        const Profile& prof );
    ProfileEditorDialog(MetaFactory* fact );
	~ProfileEditorDialog();
    Profile profile()const;

	QString prof_name();
	QString prof_type();

public slots:
	void accept();
	//void slotOk();
	void slotCancel();

	void slotDevice(int id);

	void slotConn(int id);

private:
    void initUI();

    MetaFactory* m_fact;
    EditBase* m_base;
    QTabWidget* m_tab;
    QHBoxLayout* m_lay;
    Profile m_prof;

	QLineEdit *name_line;
	QComboBox *device_box;

	QWidget *plugin_base;
	ProfileEditorPlugin *plugin_plugin;
	QHBoxLayout *plugin_layout;

	enum Ids
	{
		id_parity_odd,
		id_parity_even,
		id_flow_hw,
		id_flow_sw,
		id_baud_115200,
		id_baud_57600,
		id_baud_38400,
		id_baud_19200,
		id_baud_9600,
		id_term_vt100,
		id_term_vt220,
		id_term_ansi,
		id_size_small,
		id_size_medium,
		id_size_large
	};
};

#endif
