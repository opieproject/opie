#ifndef __OPIE_OLED_H__
#define __OPIE_OLED_H__

#include <qwidget.h>
#include <qcolor.h>

class QPixmap;

#define _QTE_IS_TOO_DUMB_TO_DRAW_AN_ARC

class OLedBox : public QWidget {
	Q_OBJECT
	
public:
	OLedBox ( const QColor &col = red, QWidget *parent = 0, const char *name = 0 );
	virtual ~OLedBox ( );
	
	QColor color ( ) const;
	bool isOn ( ) const;

	virtual QSize sizeHint ( ) const;
	
public slots:
	void toggle ( );
	void setOn ( bool on );
	void setColor ( const QColor &col );

signals:
	void toggled ( bool );

protected:
	virtual void paintEvent ( QPaintEvent *e );
	virtual void resizeEvent ( QResizeEvent *e );
	
	virtual void mousePressEvent ( QMouseEvent *e );

private:
	void drawLed ( QPixmap *, const QColor &col );

private:
	QPixmap *m_pix [2];	
	
	QColor m_color;
	bool m_on;
	
#ifdef _QTE_IS_TOO_DUMB_TO_DRAW_AN_ARC	
	static QPixmap *s_border_pix;
#endif
};

#endif
