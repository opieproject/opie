#include "oclickablelabel.h"
#include <stdio.h>

OClickableLabel::OClickableLabel(QWidget* parent, 
			       const char* name, 
			       WFlags fl) :
  QLabel(parent,name,fl)
{
    textInverted=false;
    isToggle=false;
    isDown=false;
    showState(false);
    setFrameShadow(Sunken);
}

void OClickableLabel::setToggleButton(bool t) {
    isToggle=t;
}

void OClickableLabel::mousePressEvent( QMouseEvent *e ) {
    if (isToggle && isDown) {
	showState(false);
    } else {
	showState(true);
    }
}

void OClickableLabel::mouseReleaseEvent( QMouseEvent *e ) {
    if (rect().contains(e->pos()) && isToggle) isDown=!isDown;

    if (isToggle && isDown) {
	showState(true);
    } else {
	showState(false);
    }

    if (rect().contains(e->pos())) {
	if (isToggle) {
	    emit toggled(isDown);
	}
	emit clicked();
    }
}

void OClickableLabel::mouseMoveEvent( QMouseEvent *e ) {
    if (rect().contains(e->pos())) {
	if (isToggle && isDown) {
	    showState(false);
	} else {
	    showState(true);
	}
    } else {
	if (isToggle && isDown) {
	    showState(true);
	} else {
	    showState(false);
	}
    }
}

void OClickableLabel::showState(bool on) {
  if (on) {
    //setFrameShape(Panel);
    setInverted(true);
    setBackgroundMode(PaletteHighlight);
  } else {
    //setFrameShape(NoFrame);
    setInverted(false);
    setBackgroundMode(PaletteBackground);
  }
  repaint();
}

void OClickableLabel::setInverted(bool on) {
    if ( (!textInverted && on) || (textInverted && !on) ) {
	QPalette pal=palette();
	QColor col=pal.color(QPalette::Normal, QColorGroup::Foreground);
	col.setRgb(255-col.red(),255-col.green(),255-col.blue());
	pal.setColor(QPalette::Normal, QColorGroup::Foreground, col);
	setPalette(pal);
	textInverted=!textInverted;
    }
}

void OClickableLabel::setOn(bool on) {
  isDown=on;
  showState(isDown);
}
