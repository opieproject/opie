#include "backgammonview.h"

BackGammonView::BackGammonView(QCanvas* canvas,QWidget* parent)
        :QCanvasView(canvas,parent)
{
  //do nothing
}



BackGammonView::~BackGammonView()
{
  //do nothing
}



void BackGammonView::contentsMousePressEvent(QMouseEvent* e)
{
    int x=e->x();
    int y=e->y();
    emit mouse(x,y);
}
