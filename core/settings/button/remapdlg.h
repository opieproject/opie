#ifndef __REMAPDLG_H__
#define __REMAPDLG_H__

#include <opie/odevicebutton.h>

#include "remapdlgbase.h"

class QListViewItem;


class RemapDlg : public RemapDlgBase { 
    Q_OBJECT

public:
    RemapDlg ( const Opie::ODeviceButton *b, bool hold, QWidget* parent = 0, const char* name = 0 );
    ~RemapDlg ( );

	Opie::OQCopMessage message ( );

public slots:
	virtual void itemChanged ( QListViewItem * );
	virtual void textChanged ( const QString & );

private:
	Opie::OQCopMessage m_msg;
	Opie::OQCopMessage m_msg_preset;
	
	QListViewItem *m_current;
	
	QListViewItem *m_map_none;
	QListViewItem *m_map_preset;
	QListViewItem *m_map_custom;
};

#endif
