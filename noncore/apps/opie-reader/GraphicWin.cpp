
#include "GraphicWin.h"

GraphicWin::GraphicWin( QWidget *parent, const char *name, WFlags f)
  : QWidget(parent, name, f), m_isFitted(false), m_isRotated(false)
{
  QVBoxLayout* grid = new QVBoxLayout(this);
  m_scroll = new GraphicScroll(this);
  grid->addWidget(m_scroll,1);
  QHBoxLayout* b = new QHBoxLayout(grid);

  QPushButton* fitButton = new QPushButton("Rotated", this);
  connect(fitButton, SIGNAL( clicked() ), this, SLOT( slotRotate() ) );
  b->addWidget(fitButton);

  fitButton = new QPushButton("Fitted", this);
  connect(fitButton, SIGNAL( clicked() ), this, SLOT( slotFit() ) );
  b->addWidget(fitButton);

  fitButton = new QPushButton("Close", this);
  connect(fitButton, SIGNAL( clicked() ), this, SLOT( slotClosed() ) );
  b->addWidget(fitButton);
}

void GraphicWin::setImage(QImage& im)
{
  m_isRotated = m_isFitted = false;
  m_im = im;
  resetpm();
}

QImage GraphicWin::resizeimage(int w, int h)
{
  if (w*m_im.height() < h*m_im.width())
    {
      h = (m_im.height()*w + m_im.width()/2)/m_im.width();
    }
  else
    {
      w = (m_im.width()*h + m_im.height()/2)/m_im.height();
    }
  return m_im.smoothScale(w,h);
}

void GraphicWin::slotFit()
{
  m_isFitted = !m_isFitted;
  resetpm();
}

void GraphicWin::slotRotate()
{
  m_isRotated = !m_isRotated;
  resetpm();
}

void GraphicWin::resetpm()
{
  QImage im;
  if (m_isFitted)
    {
      int w, h;
      if (m_isRotated)
	{
	  w = m_scroll->height()-5;
	  h = m_scroll->width()-5;
	}
      else
	{
	  w = m_scroll->width()-5;
	  h = m_scroll->height()-5;
	}
      im = resizeimage(w, h);
    }
  else
    {
      im = m_im;
    }
  QPixmap pc;
  if (m_isRotated)
    {
      QWMatrix m;
      m.rotate(90);
      QPixmap pc1;
      pc1.convertFromImage(im);
      pc = pc1.xForm(m);
    }
  else
    {
      pc.convertFromImage(im);
    }
  m_scroll->setPixmap(pc);
}

/*
 Something like this should work...but doesn't quite
 smoothscale is better anyway.

void GraphicWin::resetpm()
{
  QWMatrix m;
  if (m_isRotated)
    {
      m.rotate(90);
    }
  if (m_isFitted)
    {
      double sw = (width()-5.0)/(m_isRotated ? m_im.height() : m_im.width());
      double sh = (height()-5.0)/(m_isRotated ? m_im.width() : m_im.height());
      if (sw < sh)
	{
	  m.scale(sw, sw);
	}
      else
	{
	  m.scale(sh, sh);
	}
    }
  QPixmap pc = m_im.xForm(m);
  m_scroll->setPixmap(pc);
}
*/
