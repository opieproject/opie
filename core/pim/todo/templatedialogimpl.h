#ifndef OPIE_TEMPLATE_DIALOG_IMPL_H
#define OPIE_TEMPLATE_DIALOG_IMPL_H

#include <qlistview.h>

#include <opie/otodo.h>

#include "templatedialog.h"


namespace Todo {

    class MainWindow;
    class TemplateManager;
    class TemplateDialogImpl : public Todo::TemplateDialog {
        Q_OBJECT
    public:
        TemplateDialogImpl( MainWindow* win, TemplateManager* );
        ~TemplateDialogImpl();

    private:
        MainWindow* m_win;
        TemplateManager* m_man;
private slots:
        void slotAdd();
        void slotRemove();
        void slotEdit();
        void slotReturn();
        void slotClicked(QListViewItem*);
    };
};

#endif
