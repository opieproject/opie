
#ifndef FIELD_H
#define FIELD_H

#include <qwidget.h>
#include <qpixmap.h>

#define SIZE   28

class Field:public QWidget
{
  Q_OBJECT
  public:
    Field(QWidget*,int);
    void draw(QImage*);
  signals:
    void click(int);
  protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
  private:
    int number;
    QPixmap *pixmap;

};

#endif
