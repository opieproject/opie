#ifndef __OPIE_LIQUID_SET_H__
#define __OPIE_LIQUID_SET_H__

#include <qdialog.h>
#include <qcolor.h>

class QLabel;
class OColorButton;
class QSlider;

class LiquidSettings : public QWidget {
	Q_OBJECT
	
public:
	LiquidSettings ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );

public slots:
	void changeType ( int t );
	void changeShadow ( bool b );
	void changeFlat ( bool b );

	virtual bool writeConfig ( );
	
private:
	int    m_type;
	bool   m_shadow;
	bool   m_flat;
	
	QSlider *     m_opacsld;
	QSlider *     m_contsld;
	QLabel *      m_menulbl;
	QLabel *      m_textlbl;
	QLabel *      m_opaclbl;
	OColorButton *m_menubtn;
	OColorButton *m_textbtn;
};
#endif
