#include "function_keyboard.h"
#include <qsizepolicy.h> 

FunctionKeyboard::FunctionKeyboard(QWidget *parent) : 
    QFrame(parent), numRows(2), numCols(15), 
    pressedRow(0), pressedCol(0) {

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

}

FunctionKeyboard::~FunctionKeyboard() {

}

void FunctionKeyboard::paintEvent(QPaintEvent *e) {

    QPainter p(this);
    p.setClipRect(e->rect());
    p.fillRect(0, 0, width(), height(), QColor(255,255,255));

    /* those decimals do count! becomes short if use plain int */
    for (double i = 0; i <= width(); i += keyWidth) {

        p.setPen(QColor(0,0,0));
        p.drawLine((int)i, 0, (int)i, height());
    }

    for (int i = 0; i <= height(); i += height()/numRows) {

        p.setPen(QColor(0,0,0));
        p.drawLine(0, i, width(), i);
    }

}

void FunctionKeyboard::paintKey(int row, int col) {

    QPainter p(this);
    p.fillRect(QRect(QPoint(col * keyWidth + 1, row * keyHeight + 1), 
                     QPoint((col + 1) * keyWidth - 1, row * keyHeight + keyHeight- 1)), 
               (pressedRow != -1 && pressedCol != -1 ) ? QColor(97,119,155) : QColor(255,255,255));
}

void FunctionKeyboard::mousePressEvent(QMouseEvent *e) {

    pressedRow = e->y() / keyHeight;
    pressedCol = e->x() / keyWidth;

    paintKey(pressedRow, pressedCol);
}

void FunctionKeyboard::mouseReleaseEvent(QMouseEvent *) {

    if (pressedRow != -1 && pressedRow != -1) {

        int row = pressedRow; pressedRow = -1;
        int col = pressedCol; pressedCol = -1;
        paintKey(row, col);
    }
}


void FunctionKeyboard::resizeEvent(QResizeEvent*) {

    /* set he default font height/width */
    QFontMetrics fm=fontMetrics();
    keyHeight = fm.lineSpacing() + 2;
    keyWidth = (double)width()/numCols;

}

QSize FunctionKeyboard::sizeHint() const {

    return QSize(width(), keyHeight * numRows + 1);
}

