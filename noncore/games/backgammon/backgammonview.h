#ifndef BACKGAMMON_VIEW_H
#define BACKGAMMON_VIEW_H

#include <qcanvas.h>

class BackGammonView : public QCanvasView
{
    Q_OBJECT
public:
    BackGammonView(QCanvas* canvas,QWidget* parent);
    ~BackGammonView();
signals:
    void mouse(int,int);
private:
    void contentsMousePressEvent(QMouseEvent* e);
};

#endif //BACKGAMMON_VIEW_H