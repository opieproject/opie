#include "ofilelistview.h"
#include "olocallister.h"

#include "odefaultfactories.h"

extern "C" {

    OFileView* newFileListView( OFileSelector* sel, QWidget* par ) {
        return new OFileListView(par, sel );
    }
    OLister* newLocalLister( OFileSelector* sel) {
        return new OLocalLister( sel );
    }

};
