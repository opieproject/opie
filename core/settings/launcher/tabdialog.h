#ifndef __TABDIALOG_H__
#define __TABDIALOG_H__

#include <qdialog.h>
#include "tabconfig.h"

class QButtonGroup;
class OFontSelector;
class SampleView;
class OColorButton;

class TabDialog : public QDialog {
	Q_OBJECT
public:
	TabDialog ( const QPixmap *tabicon, const QString &tabname, TabConfig &cfg, QWidget *parent = 0, const char *name = 0, bool modal = false, WFlags = 0 );
	virtual ~TabDialog ( );	

protected slots:
	void iconSizeClicked ( int );
	void fontClicked ( const QFont & );
	void bgTypeClicked ( int );
	void colorClicked ( const QColor & );

private:
	QWidget *createBgTab ( QWidget *parent );
	QWidget *createFontTab ( QWidget *parent );
	QWidget *createIconTab ( QWidget *parent );
	
	
private:
	SampleView *m_sample;
	QButtonGroup *m_iconsize;
	OFontSelector *m_fontselect;
	OColorButton *m_solidcolor;
	
	QButtonGroup *m_bgtype;
};


#endif