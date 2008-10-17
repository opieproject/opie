#ifndef OPIE_DATEBOOK_TEMPLATE_MANAGER_ZECKE_H_RANDOM_CAUSE_BJARNE
#define OPIE_DATEBOOK_TEMPLATE_MANAGER_ZECKE_H_RANDOM_CAUSE_BJARNE

#include <qobject.h>

#include <opie/oevent.h>

#include "managertemplate.h"

class QListView;
namespace Datebook {
    /**
     * The OEvent Template Manager
     */
    class Editor;
    class TemplateManager : public ManagerTemplate<OEvent> {
    public:
        TemplateManager();
        ~TemplateManager();

        bool save();
        bool load();
    };
    /**
     * a QDialog for editing it
     */
    class TemplateDialog : public QDialog {
        Q_OBJECT
    public:
        TemplateDialog(const TemplateManager& man, Editor*);
        ~TemplateDialog();

        TemplateManager manager()const;

    private slots:
        void init( const TemplateManager& );
        void slotAdd();
        void slotEdit();
        void slotRename();
        void slotRemove();

    private:
        QListView* m_view;
        Editor* m_edit;
    };
}

#endif
