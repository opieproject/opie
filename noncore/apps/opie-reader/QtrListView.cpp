#include "QtrListView.h"

void QtrListView::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
	case Key_Space:
	    e->accept();
	    emit OnCentreButton(currentItem());
	    break;
//	case Key_Enter:
//	case Key_Return:
	case Key_F33:
	    e->accept();
	    emit OnOKButton(currentItem());
	    break;
	case Key_Escape:
	    e->accept();
	    emit OnCancelButton();
	    break;
	default:
	    QListView::keyPressEvent(e);
    }
}
