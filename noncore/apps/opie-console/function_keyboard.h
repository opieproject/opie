#ifndef OPIE_FUNCTION_KEYBOARD_H
#define OPIE_FUNCTION_KEYBOARD_H

#include <qpe/config.h>
#include <qframe.h>
#include <qpainter.h>
#include <qmap.h>

class FKey {

public:

    FKey(): qcode(0), unicode(0) {};
    FKey(const QString &l, ushort q, ushort u): label(l), qcode(q), unicode(u) {};

    QString getL() { return label; }
    ushort getQ() { return qcode; }
    ushort getU() { return unicode; }


private:

    QString label;
    ushort qcode;
    ushort unicode;
};

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

signals:

    void keyPressed(ushort, ushort, bool, bool, bool);

private:

    void loadDefaults();


private:

    // thie key for the map is the row/col
    QMap<QString, FKey> keys;

    uint numRows;
    uint numCols;
    uint keyHeight;
    double keyWidth; // decimal point matters!

    int pressedRow, pressedCol;

    QObject *parent;

};

#endif
