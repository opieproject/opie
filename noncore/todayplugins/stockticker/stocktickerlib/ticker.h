#ifndef TICKER_H
#define TICKER_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qframe.h>
#include <qlineedit.h>

class Ticker : public QFrame {
    Q_OBJECT

public:
    Ticker( QWidget* parent=0 );
    ~Ticker();
    void setText( const QString& text ) ;
signals:
    void mousePressed();
protected:
    void timerEvent( QTimerEvent * );
    void drawContents( QPainter *p );
    void mouseReleaseEvent ( QMouseEvent *);
private:
    QString scrollText;
    QPixmap scrollTextPixmap;
    int pos;//, pixelLen;
};

#endif
