#ifndef PROFILE_EDITOR_DIALOG
#define PROFILE_EDITOR_DIALOG

#include <qdialog.h>

#include "profile.h"

class MetaFactory;
class EditBase;
class QTabWidget;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QLabel;
class ProfileEditorPlugin;

class ProfileEditorDialog : public QDialog {
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
    void slotDevice(int id);

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
};

#endif
