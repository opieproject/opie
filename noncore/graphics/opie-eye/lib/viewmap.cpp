#include <iface/dirview.h>


namespace {
    ViewMap  *_viewMap = 0;
    PDirView *_dirView = 0;
}


ViewMap *viewMap() {
    if ( !_viewMap )
        _viewMap = new ViewMap;

    return _viewMap;
}


PDirView*  currentView(){
    return _dirView;
}


void  setCurrentView(  PDirView* view ) {
    _dirView = view;
}
