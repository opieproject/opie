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

	QString conn_device();

	QString term_type();

public slots:
	void accept();
	//void slotOk();
	void slotCancel();

	void slotDevice(int id);

private:
    void initUI();

    MetaFactory* m_fact;
    EditBase* m_base;
    QTabWidget* m_tab;
    QHBoxLayout* m_lay;
    Profile m_prof;

	//QLabel *frame_number;
	QLineEdit *name_line;
	//QLineEdit *frame_device_line, *frame_number_line;
	QComboBox *terminal_box, *speed_box, *device_box;
	QWidget *plugin_base, *plugin_plugin;
	QHBoxLayout *plugin_layout;
};

#endif
