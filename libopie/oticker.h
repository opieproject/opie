#ifndef OTICKER_H
#define OTICKER_H

#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qslider.h>
#include <qframe.h>
#include <qlineedit.h>

class OTicker : public QFrame {
    Q_OBJECT

public:
    OTicker( QWidget* parent=0 );
    ~OTicker();
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
