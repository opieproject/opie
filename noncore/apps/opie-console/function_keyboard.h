#ifndef OPIE_FUNCTION_KEYBOARD_H
#define OPIE_FUNCTION_KEYBOARD_H

#include <qframe.h>
#include <qpainter.h>

class FunctionKeyboard : public QFrame {
    Q_OBJECT

public:
    FunctionKeyboard(QWidget *parent = 0);
    ~FunctionKeyboard();

    void paintEvent(QPaintEvent *);
    void paintKey(int, int);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);


    QSize sizeHint() const;

private:
    uint numRows;
    uint numCols;
    uint keyHeight;
    double keyWidth; // decimal point matters!

    int pressedRow, pressedCol;

};

#endif
