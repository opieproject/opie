#ifndef OPIE_DATEBOOK_TEMPLATE_MANAGER_ZECKE_H_RANDOM_CAUSE_BJARNE
#define OPIE_DATEBOOK_TEMPLATE_MANAGER_ZECKE_H_RANDOM_CAUSE_BJARNE

#include <qobject.h>

#include <opie/oevent.h>

#include "managertemplate.h"

namespace Datebook {
    /**
     * The OEvent Template Manager
     */
    class TemplateManager : public ManagerTemplate<OEvent> {
    public:
        TemplateManager();
        ~TemplateManager();

    private:
        virtual bool doSave();
        virtual bool doLoad();
    };
    /**
     * a QDialog for editing it
     */
    class TemplateDialog : public QDialog {
        Q_OBJECT
    public:
        TemplateDialog(const TemplateManager& man);
        ~TemplateDialog();

        virtual TemplateManager manager()const;
    };
}

#endif
