#include "ofilelistview.h"
#include "ofileiconview.h"
#include "olocallister.h"


#include "odefaultfactories.h"

extern "C" {


    OFileView* newFileIconView( OFileSelector* sel, QWidget* parent ) {
        return new OFileIconView( parent, sel );
    }

    OFileView* newFileListView( OFileSelector* sel, QWidget* par ) {
        return new OFileListView(par, sel );
    }

    OLister* newLocalLister( OFileSelector* sel) {
        return new OLocalLister( sel );
    }

};
