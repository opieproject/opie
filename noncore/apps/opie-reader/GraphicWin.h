#ifndef __GRAPHICWIN_H
#define __GRAPHICWIN_H

#include <qscrollview.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qwmatrix.h>

class GraphicScroll : public QScrollView
{
  Q_OBJECT
  QWidget* m_picture;
 protected:
  void hideEvent( QHideEvent * p)
    {
      m_picture->setFixedSize(1,1);	    
    }
 public:
  GraphicScroll( QWidget *parent=0, const char *name=0, WFlags f = 0)
    : QScrollView(parent, name, f)
    {
      m_picture = new QWidget(viewport());
      addChild(m_picture);
    }
  void setPixmap(QPixmap& pm)
    {
      m_picture->setFixedSize(pm.size());
      m_picture->setBackgroundPixmap(pm);
    }
};


class GraphicWin : public QWidget
{
  Q_OBJECT
    
  GraphicScroll* m_scroll;
  QImage m_im;
  QImage resizeimage(int w, int h);
  void resetpm();
  bool m_isFitted, m_isRotated;
signals:
  void Closed();
private slots:
  void slotClosed() { emit Closed(); }
  void slotFit();
  void slotRotate();
public:
  void setPixmap(QPixmap& pm) { m_scroll->setPixmap(pm); }
  void setImage(QImage& im);
  GraphicWin( QWidget *parent=0, const char *name=0, WFlags f = 0);
};

#endif
