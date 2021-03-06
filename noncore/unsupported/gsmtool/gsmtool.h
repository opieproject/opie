#ifndef EXAMPLE_H
#define EXAMPLE_H
#include "gsmtoolbase.h"

#include <termios.h>

#include <gsmlib/gsm_me_ta.h>
#include <gsmlib/gsm_sorted_sms_store.h>

class GSMTool : public GSMToolBase
{
    Q_OBJECT

public:
    GSMTool( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~GSMTool();

protected:
    void timerEvent(QTimerEvent *te );

private slots:
	void doConnectButton();
	void doScanButton();
        void doTabChanged();
	void doSMSStoreChanged();
	void doSMSTypeChanged();
	void doSelectedSMSChanged(QListViewItem *);
	void doSMSSendButton();
	void doSMSDeleteButton();
	void doNewSMSClearButton();
	void doNewSMSSaveButton();
	void doNewSMSSendButton();
private:
 static const speed_t baudrates[];
 int devicelocked;
 int timerid;

 gsmlib::MeTa *me;
 gsmlib::SortedSMSStoreRef sms_store;

 char *devicename;
 speed_t baudrate;

 int lockDevice( );
 void unlockDevice( );

 void setConnected( bool conn );
};

#endif // EXAMPLE_H
