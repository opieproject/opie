#ifndef __QBUSYBAR_H__
#define __QBUSYBAR_H__

#include <qwidget.h>

class QColor;
class QTimer;

class QBusyBar : public QWidget {
	Q_OBJECT

public:
	QBusyBar ( QWidget *parent = 0, const char *name = 0, int flags = 0 );
	virtual ~QBusyBar ( );

	bool isBusy ( ) const;

	void setParameters ( int div, int fade, int speed );

public slots:
	void beginBusy ( );
	void endBusy ( );
	
	void setBusy ( bool b ); 

protected:
	virtual void paintEvent ( QPaintEvent *e );
	
private slots:
	void slotTimeout ( );
	
private:
	QTimer *m_timer;
	int     m_busy;

	int     m_div;
	int     m_pos;  // 0 .. 2 * m_div - 1
	int     m_fade; // 0 .. m_div - 1	
	int 	m_speed;	
	QColor *m_fadecols;
};

#endif
