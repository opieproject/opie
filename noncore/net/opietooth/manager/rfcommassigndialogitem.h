#ifndef RFCOMMDIALOGITEM
#define RFCOMMDIALOGITEM

#include "rfcommdialogitembase.h"

#include <qwidget.h>

namespace OpieTooth {

    class RfcommDialogItem : public RfcommDialogItemBase {
	
	Q_OBJECT

    public:
	RfcommDialogItem( QWidget* parent = 0,  const char* name = 0, WFlags fl = 0 );
	~RfcommDialogItem();
        
	// number if the rfcomm device
	int ident();
	// devices mac address
	QString mac();
	int channel();	
	QString comment();

        void setIdent( int ident );
	void setMac( const QString& mac  );
	void setChannel( int channel );
	void setComment( const QString& comment );

    private:


	int m_ident;
   
};


}

#endif
