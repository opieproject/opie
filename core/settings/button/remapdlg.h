#ifndef __REMAPDLG_H__
#define __REMAPDLG_H__

#include <opie2/odevicebutton.h>

#include "remapdlgbase.h"

class QListViewItem;


class RemapDlg : public RemapDlgBase { 
    Q_OBJECT

public:
    RemapDlg ( const Opie::Core::ODeviceButton *b, bool hold, QWidget* parent = 0, const char* name = 0 );
    ~RemapDlg ( );

	Opie::Core::OQCopMessage message ( );

public slots:
	virtual void itemChanged ( QListViewItem * );
	virtual void textChanged ( const QString & );
	
private slots:
	void delayedInit ( );

private:
	Opie::Core::OQCopMessage m_msg;
	Opie::Core::OQCopMessage m_msg_preset;
	
	QListViewItem *m_current;
	
	QListViewItem *m_map_none;
	QListViewItem *m_map_preset;
	QListViewItem *m_map_custom;
	QListViewItem *m_map_show;
};

#endif
