#ifndef __OPIE_LIQUID_SET_H__
#define __OPIE_LIQUID_SET_H__

#include <qdialog.h>
#include <qcolor.h>

class QLabel;
class QToolButton;
class QSlider;

class LiquidSettings : public QWidget {
	Q_OBJECT
	
public:
	LiquidSettings ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );

public slots:
	void changeType ( int t );
	void changeMenuColor ( const QColor &col );
	void changeTextColor ( const QColor &col );
	void changeShadow ( bool b );
	void changeDeco ( bool b );

	virtual bool writeConfig ( );
	
private:
	QColor m_menucol;
	QColor m_textcol;
	int    m_type;
	bool   m_shadow;
	bool   m_deco;
	
	QSlider *    m_opacsld;
	QSlider *    m_contsld;
	QLabel *     m_menulbl;
	QLabel *     m_textlbl;
	QLabel *     m_opaclbl;
	QToolButton *m_menubtn;
	QToolButton *m_textbtn;
};
#endif
