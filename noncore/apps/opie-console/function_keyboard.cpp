#include "function_keyboard.h"
#include <qsizepolicy.h> 
#include <qevent.h>
#include <qapplication.h>

FunctionKeyboard::FunctionKeyboard(QWidget *parent) : 
    QFrame(parent), numRows(1), numCols(11), 
    pressedRow(0), pressedCol(0) {

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    Config conf("opie-console-keys");
    conf.setGroup("keys");
    for (int r = 0; r < numRows; r++)
        for (int c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            QStringList value_list = conf.readListEntry( handle, '|');

            if (value_list.isEmpty()) continue;

            keys.insert(

                 handle, 
                 FKey (value_list[0], value_list[1].toUShort(), value_list[2].toUShort())
            );
        }
    qWarning("loaded %d keys", keys.count());



}

FunctionKeyboard::~FunctionKeyboard() {

}

void FunctionKeyboard::paintEvent(QPaintEvent *e) {

    QPainter p(this);
    p.setClipRect(e->rect());
    p.fillRect(0, 0, width(), height(), QColor(255,255,255));

    p.setPen(QColor(0,0,0));

    /* those decimals do count! becomes short if use plain int */
    for (double i = 0; i <= width(); i += keyWidth) {

        p.drawLine((int)i, 0, (int)i, height());
    }

    // sometimes the last line doesnt get drawn
    p.drawLine(width() -1, 0, width() -1, height());

    for (int i = 0; i <= height(); i += keyHeight) {

        p.drawLine(0, i, width(), i);
    }

    for (int r = 0; r < numRows; r++) {
        for (int c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            if (keys.contains(handle)) {

                p.drawText(
                                c * keyWidth + 1, r * keyHeight + 1, 
                                keyWidth, keyHeight, 
                                Qt::AlignHCenter | Qt::AlignVCenter, 
                                keys[handle].getL()
                );
            }
        }
    }
}

void FunctionKeyboard::paintKey(int row, int col) {

    QPainter p(this);
    
    p.fillRect(QRect(QPoint(col * keyWidth + 1, row * keyHeight + 1), 
                     QPoint((col + 1) * keyWidth - 1, row * keyHeight + keyHeight- 1)), 
               (pressedRow != -1 && pressedCol != -1 ) ? QColor(97,119,155) : QColor(255,255,255));
    p.drawText(
                    col * keyWidth + 1, row * keyHeight + 1, 
                    keyWidth, keyHeight, 
                    Qt::AlignHCenter | Qt::AlignVCenter, 
                    keys["r" + QString::number(row) + "c" + QString::number(col)].getL()
    );

}

void FunctionKeyboard::mousePressEvent(QMouseEvent *e) {

    pressedRow = e->y() / keyHeight;
    pressedCol = (int) (e->x() / keyWidth);

    paintKey(pressedRow, pressedCol);

    // emit that sucker!
    FKey k = keys["r" + QString::number(pressedRow) + "c" + QString::number(pressedCol)];
    //QWSServer::sendKeyEvent(k.getU(), k.getQ(), 0, 1, 0);
    //qwsServer->sendKeyEvent(k.getU(), k.getQ(), 0, 1, 0);
    //qwsServer->sendKeyEvent(0x41, 0, 0, 1, 0);

    QKeyEvent ke(QEvent::KeyPress, k.getQ(), k.getU(), 0);
    QApplication::sendEvent(this, &ke); 

}

void FunctionKeyboard::mouseReleaseEvent(QMouseEvent *) {

    if (pressedRow != -1 && pressedRow != -1) {

        int row = pressedRow; pressedRow = -1;
        int col = pressedCol; pressedCol = -1;
        paintKey(row, col);

        FKey k = keys["r" + QString::number(row) + "c" + QString::number(col)];
        //QWSServer::sendKeyEvent(k.getU(), k.getQ(), 0, 0, 0);
        //qwsServer->sendKeyEvent(k.getU(), k.getQ(), 0, 0, 0);
        QKeyEvent ke(QEvent::KeyRelease, k.getQ(), k.getU(), 0);
        QApplication::sendEvent(this, &ke); 
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

