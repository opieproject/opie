
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

    void setFrame(QImage*);
    void setPicture(QImage*);
    void setPattern(QImage*);
    void setLabel(const QString &);

  signals:
    void click(int);

  protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);

  private:
    void draw();

    int number;

    // pixmap = pattern + label + picture + frame;

    QImage* frame;
    QImage* picture;
    QImage* pattern;
    QString label;

    QPixmap *pixmap;

};

#endif
