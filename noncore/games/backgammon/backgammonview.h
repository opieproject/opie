#ifndef BACKGAMMONVIEW_H
#define BACKGAMMONVIEW_H

#include <qcanvas.h>

class BackGammonView : public QCanvasView
{
    Q_OBJECT
public:
    BackGammonView(QCanvas* canvas,QWidget* parent);
	~BackGammonView();
signals:
    //void mousepressed(int,int);
private:
    void contentsMousePressEvent(QMouseEvent* e);
};

#endif //BACKGAMMONVIEW_H