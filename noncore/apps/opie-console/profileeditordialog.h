#ifndef PROFILE_EDITOR_DIALOG
#define PROFILE_EDITOR_DIALOG

#include <qdialog.h>
#include <opie/otabwidget.h>

#include "profile.h"

class MetaFactory;
class EditBase;
class QTabWidget;
class QHBoxLayout;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QLabel;
class ProfileDialogWidget;

class ProfileEditorDialog : public QDialog {
    Q_OBJECT
public:
    ProfileEditorDialog(MetaFactory* fact,
                        const Profile& prof );
    ProfileEditorDialog(MetaFactory* fact );
    ~ProfileEditorDialog();
    Profile profile()const;



public slots:
    void accept();

private slots:
    void slotConActivated(const QString& );
    void slotTermActivated( const QString& );
private:
    void initUI();
    QString profName()const;
    QCString profType()const;

    MetaFactory* m_fact;
    QHBoxLayout* m_lay;
    Profile m_prof;

    QLineEdit *m_name;
    QComboBox *m_conCmb, *m_termCmb;
    QCheckBox *m_autoConnect;

    QWidget *m_tabCon, *m_tabTerm;
    ProfileDialogWidget* m_con, *m_term;
    QHBoxLayout *m_layCon, *m_layTerm;
    OTabWidget *tabWidget;
    QWidget *tabprof;
    int m_showconntab;
};

#endif
