#include "backgammonview.h"

BackGammonView::BackGammonView(QCanvas* canvas,QWidget* parent)
    :QCanvasView(canvas,parent)
{
}

BackGammonView::~BackGammonView()
{
}

void BackGammonView::contentsMousePressEvent(QMouseEvent* e)
{
    int x=e->x();
	int y=e->y();
	//emit mousepressed(x,y);
}