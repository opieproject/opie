#include "clickablelabel.h"
#include <stdio.h>

ClickableLabel::ClickableLabel(QWidget* parent = 0, 
			       const char* name = 0, 
			       WFlags fl = 0) :
  QLabel(parent,name,fl)
{
    textInverted=false;
    isToggle=false;
    isDown=false;
    showState(false);
    setFrameShadow(Sunken);
}

void ClickableLabel::setToggleButton(bool t) {
    isToggle=t;
}

void ClickableLabel::mousePressEvent( QMouseEvent *e ) {
    if (isToggle && isDown) {
	showState(false);
    } else {
	showState(true);
    }
}

void ClickableLabel::mouseReleaseEvent( QMouseEvent *e ) {
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

void ClickableLabel::mouseMoveEvent( QMouseEvent *e ) {
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

void ClickableLabel::showState(bool on) {
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

void ClickableLabel::setInverted(bool on) {
    if ( (!textInverted && on) || (textInverted && !on) ) {
	QPalette pal=palette();
	QColor col=pal.color(QPalette::Normal, QColorGroup::Foreground);
	col.setRgb(255-col.red(),255-col.green(),255-col.blue());
	pal.setColor(QPalette::Normal, QColorGroup::Foreground, col);
	setPalette(pal);
	textInverted=!textInverted;
    }
}

void ClickableLabel::setOn(bool on) {
  isDown=on;
  showState(isDown);
}
