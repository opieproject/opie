#ifndef OPIE_TEMPLATE_DIALOG_H
#define OPIE_TEMPLATE_DIALOG_H

#include <qdialog.h>
#include <qstring.h>

class QListView;
class QPushButton;
class QLabel;
class QHBox;
class QVBoxLayout;
class QLineEdit;


namespace Todo {
    class TemplateDialog : public QDialog {
        Q_OBJECT
    public:
        /* it'll be modal */
        TemplateDialog( QWidget* parent );
        virtual ~TemplateDialog();

    protected:
        QListView* listView();
        QLineEdit* edit();
private slots:
        virtual void slotAdd();
        virtual void slotRemove();
        virtual void slotEdit();
        virtual void slotReturn();

    private:
        QVBoxLayout* m_main;
        QHBox* m_btnBar;
        QListView* m_list;
        QLineEdit *m_lne;
        QPushButton* m_add;
        QPushButton* m_rem;
        QPushButton* m_edit;

    };
}

#endif
