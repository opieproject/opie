#ifndef PROFILE_EDITOR_DIALOG
#define PROFILE_EDITOR_DIALOG

#include <qdialog.h>

#include "profile.h"

class MetaFactory;
class EditBase;
class QTabWidget;
class QHBoxLayout;
class ProfileEditorDialog : public QDialog {
    Q_OBJECT
public:
    ProfileEditorDialog(MetaFactory* fact,
                        const Profile& prof );
    ProfileEditorDialog(MetaFactory* fact );
    Profile profile()const;
private:
    void initUI();
    MetaFactory* m_fact;
    EditBase* m_base;
    QTabWidget* m_tab;
    QHBoxLayout* m_lay;
    Profile m_prof;
};

#endif
