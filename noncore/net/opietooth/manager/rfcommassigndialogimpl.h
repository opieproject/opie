#ifndef RFCOMMASSIGN
#define RFCOMMASSIGN

#include "rfcommassigndialogbase.h"
#include "rfcommassigndialogitem.h"
#include "rfcommconfhandler.h"

#include <qscrollview.h>
#include <qmap.h>
#include <qvbox.h>

namespace OpieTooth {

    class RfcommAssignDialog: public RfcommAssignDialogBase {

	Q_OBJECT
	    
    public:
	
	RfcommAssignDialog( QWidget* parent = 0, const char* name = 0,bool modal = 0, WFlags fl = 0 ); 
	~RfcommAssignDialog();
	
	void loadConfig();
	void saveConfig();

	void newDevice( const QString & mac );

    private:
	QScrollView *m_scrollView;
	// how many rfcomm devices are possible
	int m_range;
	QMap< int, RfcommDialogItem* > m_itemList;
	QVBox *m_box;
	RfCommConfHandler *confHandler;
};

}

#endif
