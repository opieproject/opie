#ifndef _KHC_WIDGET_H_
#define _KHC_WIDGET_H_

#include <stdlib.h>

#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include <qwindowsystem_qws.h>

#include <qpe/qcopenvelope_qws.h>

#include "KHCWidgetBase.h"
#include "KeyNames.h"

struct KeyData
{
	KeyData(){
		key = 0;
		state = 0;
		ascii = 0;
	}
	void setData(QKeyEvent* ke) {
		key = ke->key();
		state = ke->state();
		ascii = ke->ascii();
		text = ke->text();
	}
	int key;
	int state;
	int ascii;
	QString text;
};

class KHCWidget : public KHCWidgetBase
{
	Q_OBJECT
public:
	KHCWidget(QWidget* parent=0, const char* name=0, WFlags fl=0);
	virtual ~KHCWidget();

protected:
	void closeEvent(QCloseEvent* ce);

private:
	void setLayout();
	void setHandler();

	void onPress_Org(QKeyEvent* ke);
	void onPress_Map(QKeyEvent* ke);

	bool eventFilter(QObject* o, QEvent* e);

	bool m_isEnable;
	KeyData m_OrgkeyData;
	KeyData m_MapkeyData;
private slots:
	void onClick_Gen();
	void onClick_Copy();
	void onClick_Ctrl();
};

#endif /* _KHC_WIDGET_H_ */
