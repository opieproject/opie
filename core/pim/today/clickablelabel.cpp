#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent,
			       const char* name,
			       WFlags fl) :
  QLabel(parent,name,fl)
{
    setFrameShape(NoFrame);
    setFrameShadow(Sunken);
}

void ClickableLabel::mousePressEvent( QMouseEvent *e ) {
    setFrameShape(Panel);
    repaint();
}

void ClickableLabel::mouseReleaseEvent( QMouseEvent *e ) {
    setFrameShape(NoFrame);
    repaint();
    if (rect().contains(e->pos())) {
	emit clicked();
    }
}

void ClickableLabel::mouseMoveEvent( QMouseEvent *e ) {
    if (rect().contains(e->pos())) {
	setFrameShape(Panel);
    } else {
	setFrameShape(NoFrame);
    }
}
